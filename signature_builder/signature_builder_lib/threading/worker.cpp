#include "worker.h"
#include <crypto/hash_maker.h>

#include <iostream>

#include <boost/format.hpp>

namespace builder::threading
{
    std::tuple<bool, uint64_t> Worker::getNextBlockId() const noexcept
    {
        std::lock_guard<std::mutex> lock(m_block_id_mutex);

        if (m_current_block_id < m_current_limit)
        {
            return std::make_tuple(true, m_current_block_id++);
        }

        return std::make_tuple(false, uint64_t(0));
    }

    Worker::Worker
    (
        Barrier&             barrier, 
        uint64_t&            current_limit, 
        uint64_t&            current_block_id,
        filesys::FileMapper& input_mapper, 
        filesys::FileMapper& output_mapper, 
        std::mutex&          block_id_mutex,
        std::mutex&          console_mutex
    )
        : m_barrier(barrier)
        , m_current_limit(current_limit)
        , m_current_block_id(current_block_id)
        , m_input_mapper(input_mapper)
        , m_output_mapper(output_mapper)
        , m_block_id_mutex(block_id_mutex)
        , m_console_mutex(console_mutex)
    {
    }

    void Worker::run() try
    {
        LOG(boost::format("thread [%d] started") % std::this_thread::get_id())

        while (true)
        {
            auto [status, taken_block_id] = getNextBlockId();

            if (!status)
            {
                LOG("Reached limit, notifying master");

                bool stop = m_barrier.notifyMaster();

                if (stop)
                {
                    LOG("finish")
                    return;
                }

                LOG("Waked up by master");
            }
            else
            {
                LOG(boost::format("block=[%d] | total=[%d] | limit=[%d]")
                    % taken_block_id
                    % m_input_mapper.getTotalBlocks()
                    % m_current_limit);

                auto taken_block  = m_input_mapper.getPtr(taken_block_id);
                auto result_block = m_output_mapper.getRawPtr(taken_block_id);
                auto digest       = crypto::HashMaker{ taken_block.m_ptr, taken_block.m_size }.getHash();

                std::memcpy(result_block, digest.data(), digest.size());
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::lock_guard<std::mutex> lock(m_console_mutex);

        std::cout << boost::format("failed - [%s]") % ex.what() << std::endl;
    }
}
