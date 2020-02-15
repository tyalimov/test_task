#include "thread_manager.h"
#include "hash_maker.h"

#include <iostream>
#include <boost/format.hpp>

namespace builder::threading
{
    void ThreadManager::mapFiles()
    {
        m_input_mapper.map();
        m_output_mapper.map();
    }

    void ThreadManager::createWorkers()
    {
        for (auto i = uint32_t(0); i < m_workers_count; i++)
        {
            m_workers.emplace_back
            (
                Worker
                {
                    m_console_mutex,
                    m_current_block_id,
                    m_input_mapper,
                    m_output_mapper
                }
            );
        }
    }

    void ThreadManager::createThreads()
    {
        for (auto& worker : m_workers)
        {
            m_threads.emplace_back
            (
                std::thread { &Worker::run, &worker }
            );
        }
    }

    void ThreadManager::joinAll()
    {
        for (auto& thread : m_threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }

    void ThreadManager::updateProgressBar() const
    {
        auto processed_blocks = m_current_block_id.load();

        double percentage = (double(processed_blocks) / double(m_total_blocks)) * 100.0;
        
        std::cout << boost::format("\rblocks processed: %d/%d | %d%%       ")
            % processed_blocks
            % m_total_blocks
            % percentage;
    }

#ifdef _DEBUG
    void ThreadManager::printStats()
    {
    }
#endif

    uint32_t ThreadManager::getOptimalWorkersCount()
    {
        auto hardware_concurrency = std::thread::hardware_concurrency();

        return hardware_concurrency > 0 
            ? hardware_concurrency
            : 1;
    }

    uint64_t ThreadManager::getBlocksCount(const utils::Path &input, uint64_t block_size)
    {
        auto file_size = static_cast<uint64_t>(utils::fs::file_size(input));

        return utils::AlignGreater(file_size, block_size);
    }

    ThreadManager::ThreadManager(const utils::Path &input, const utils::Path &output, uint64_t block_size, uint32_t workers_count)
        : m_workers()
        , m_threads()
        , m_stop_pool(false)
        , m_input_mapper(input, block_size)
        , m_output_mapper(output, SHA512_DIGEST_SIZE)
        , m_workers_count(workers_count)
        , m_total_blocks(getBlocksCount(input, block_size))
        , m_current_block_id(0)
    {
        mapFiles();
        createWorkers();
        DBG_PRINT_STATS()
        createThreads();
    }

    ThreadManager::ThreadManager(const utils::Path &input, const utils::Path &output, uint64_t block_size)
        : m_workers()
        , m_threads()
        , m_stop_pool(false)
        , m_input_mapper(input, block_size)
        , m_output_mapper(output, SHA512_DIGEST_SIZE)
        , m_workers_count(getOptimalWorkersCount())
        , m_total_blocks(getBlocksCount(input, block_size))
        , m_current_block_id(0)
    {
        mapFiles();
        createWorkers();
        DBG_PRINT_STATS()
        createThreads();
    }

    void ThreadManager::run() try
    {
        while (m_current_block_id.load() < m_input_mapper.getTotalBlocks())
        {
            UPDATE_PROGRESS_BAR()

            // TODO: Убрать это позорище
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        joinAll();
    }
    catch (const std::exception& ex)
    {
        std::lock_guard<std::mutex> lock(m_console_mutex);

        std::cout << boost::format("main thread failed - [%s]")
            % ex.what();
    }
}
