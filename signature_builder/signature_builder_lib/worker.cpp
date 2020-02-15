#include "worker.h"
#include "hash_maker.h"

#include <iostream>
#include <boost/format.hpp>

namespace builder::threading
{
    Worker::Worker
    (
        std::mutex&           console_mutex,
        std::atomic_uint64_t& current_block_id,
        filesys::FileMapper&  input_mapper,
        filesys::FileMapper&  output_mapper
    )
        : m_console_mutex(console_mutex)
        , m_current_block_id(current_block_id)
        , m_input_mapper(input_mapper)
        , m_output_mapper(output_mapper)
    {
    }

    void Worker::run() try
    {

        LOG(boost::format("thread [%d] started") % std::this_thread::get_id())

        while (m_current_block_id.load() < m_input_mapper.getTotalBlocks())
        {
            uint64_t taken_block_id = m_current_block_id++;

            if (taken_block_id >= m_input_mapper.getTotalBlocks())
            {
                break;
            }

            LOG(boost::format("[%d] get block - [%d] of [%d]") 
                % std::this_thread::get_id() 
                % taken_block_id 
                % m_input_mapper.getTotalBlocks())
            
            filesys::MappedBlock taken_block  = m_input_mapper.getPtr(taken_block_id);
            filesys::MappedBlock result_block = m_output_mapper.getPtr(taken_block_id);
            SHA512_HASH digest                = crypto::HashMaker(taken_block.m_ptr, taken_block.m_size).getHash();

            //std::memcpy(result_block.m_ptr, digest.bytes, result_block.m_size);
        }
    }
    catch (const std::exception& ex)
    {
        std::lock_guard<std::mutex> lock(m_console_mutex);

        std::cout << boost::format("thread [%d] failed - [%s]")
            % std::this_thread::get_id()
            % ex.what() 
        << std::endl;
    }
}
