#include "thread_manager.h"
#include "hash_maker.h"


#include <iostream>
#include <boost/format.hpp>

namespace builder::threading
{
    void ThreadManager::initialize()
    {
        m_input_file_size     = static_cast<uint64_t>(utils::fs::file_size(m_input_file_name));
        m_max_hash_task_count = getMaxActiveBlocksCount(m_block_size);
        m_total_blocks        = getBlocksCount(m_input_file_size, m_block_size);

        m_output_writer = filesys::OutputWriter{ m_output_file_name, m_total_blocks };
    }

    void ThreadManager::createWorkers()
    {
        for (auto i = uint32_t(0); i < m_workers_count; i++)
        {
            m_workers.emplace_back
            (
                Worker
                {
                    m_hash_task_queue,
                    m_flush_task_queue,
                    m_console_mutex,
                    m_stop_pool
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

    void ThreadManager::readBlocks()
    {
        uint32_t i{ 0 };
        
        while (m_input_reader && i++ < kMaxBlocksToRead)
        {
            utils::BinaryBlock block;
            m_input_reader >> block;
            m_hash_task_queue.push(Task{ block });
        }
        
        if (!m_input_reader)
        {
            m_hash_task_queue.stop();
        }
    }

    void ThreadManager::flushBlocks()
    {
#ifdef _DEBUG
        {
            std::lock_guard<std::mutex> lock(m_console_mutex);
            std::cout << "Main thread started dlushing blocks\n";
        }
#endif

        std::vector<Task> tasks;
        
        auto status = m_flush_task_queue.tryPop(tasks, kMaxBlocksToFlush);
        
        if (!status)
        {
            return;
        }
        
        std::vector<utils::BinaryBlock> blocks;
        blocks.reserve(tasks.size());
        
        for (const auto& task : tasks)
        {
            blocks.push_back(task.block);
        }
        
        m_output_writer << blocks;
        m_blocks_processed += blocks.size();
    }

    void ThreadManager::calcHashes()
    {
    }

    void ThreadManager::updateProgressBar() const
    {
        double percentage = (double(m_blocks_processed) / double(m_total_blocks)) * 100.0;
        
        std::cout << boost::format("\rblocks processed: %d/%d | %d%%       ")
            % m_blocks_processed
            % m_total_blocks
            % percentage;
    }

#ifdef _DEBUG
    void ThreadManager::printStatistics()
    {
        std::cout << boost::format
        (
            "m_input_file_size ----- [%d]\n"   \
            "m_total_blocks -------- [%d]\n"   \
            "m_block_size ---------- [%d]\n"   \
            "m_workers_count ------- [%d]\n"   \
            "m_max_hash_task_count - [%d]\n"   \
        )
            % m_input_file_size
            % m_total_blocks
            % m_block_size
            % m_workers_count
            % m_max_hash_task_count;

    }
#endif

    uint32_t ThreadManager::getOptimalWorkersCount()
    {
        auto hardware_concurrency = std::thread::hardware_concurrency();

        return hardware_concurrency > 0 
            ? hardware_concurrency
            : 1;
    }

    uint64_t ThreadManager::getBlocksCount(uint64_t file_size, uint64_t block_size)
    {
        return (file_size % block_size) ? (file_size / block_size + 1) : (file_size / block_size);
    }

    uint64_t ThreadManager::getMaxActiveBlocksCount(uint64_t block_size) const
    {
        return kMaxAvailableMemory / block_size;
    }

    ThreadManager::ThreadManager(const utils::Path &input, const utils::Path &output, uint64_t block_size, uint32_t workers_count)
        : m_stop_pool(false)
        , m_input_reader(input, block_size)
        , m_workers_count(workers_count)
        , m_blocks_processed(0)
        , m_block_size(block_size)
        , m_input_file_name(input)
        , m_output_file_name(output)
    {
        initialize();
        createWorkers();
        
#ifdef _DEBUG
        printStatistics();
#endif

        createThreads();
    }

    ThreadManager::ThreadManager(const utils::Path &input, const utils::Path &output, uint64_t block_size)
        : m_stop_pool(false)
        , m_input_reader(input, block_size)
        , m_workers_count(getOptimalWorkersCount())
        , m_blocks_processed(0)
        , m_block_size(block_size)
        , m_input_file_name(input)
        , m_output_file_name(output)
    {
        initialize();
        createWorkers();
        
#ifdef _DEBUG
        printStatistics();
#endif

        createThreads();
    }

    void ThreadManager::run() try
    {
        while(m_input_reader)
        {
            readBlocks();
        }

        m_hash_task_queue.stop();

        joinAll();

        while(m_flush_task_queue.size() != 0)
        {
            flushBlocks();
        }
    }
    catch (const std::exception& ex)
    {
        std::lock_guard<std::mutex> lock(m_console_mutex);

        std::cout << boost::format("main thread failed - [%s]")
            % ex.what();
    }
}
