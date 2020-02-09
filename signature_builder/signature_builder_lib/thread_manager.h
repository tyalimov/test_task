#pragma once

#include "utils.h"
#include "input_reader.h"
#include "worker.h"

#include <thread>
#include <mutex>
#include <atomic>

#include "task_queue.h"
#include "output_writer.h"

namespace builder::threading
{
    constexpr uint64_t Gb(int val)
    {
        return uint64_t(val) * uint64_t(1024) * uint64_t(1024);
    }

    class ThreadManager
    {
    private:

        static const uint64_t kMaxBlocksToRead     = 128;
        static const uint64_t kMaxBlocksToFlush    = 128;
        static const uint64_t kMaxAvailableMemory  = Gb(2);
        
        TaskQueue                m_hash_task_queue;
        TaskQueue                m_flush_task_queue;
        std::vector<Worker>      m_workers;
        std::vector<std::thread> m_threads;
        std::mutex               m_console_mutex;
        std::atomic_bool         m_stop_pool;           // ctor

        filesys::InputReader     m_input_reader;        // ctor
        filesys::OutputWriter    m_output_writer;       // +

        uint32_t                 m_workers_count;       // ctor
        uint64_t                 m_blocks_processed;    // ctor
        uint64_t                 m_total_blocks;        // +
        uint64_t                 m_max_hash_task_count; // +
        uint64_t                 m_block_size;          // ctor
        uint64_t                 m_input_file_size;     // +

        utils::Path              m_input_file_name;     // ctor
        utils::Path              m_output_file_name;    // ctor

        // thread management
        void initialize();
        void createWorkers();
        void createThreads();
        void joinAll();

        // task management
        void readBlocks();
        void flushBlocks();
        void calcHashes();
        void updateProgressBar() const;

#ifdef _DEBUG
        void printStatistics();
#endif

        [[nodiscard]] static uint64_t getBlocksCount(uint64_t file_size, uint64_t block_size);
        [[nodiscard]] uint64_t getMaxActiveBlocksCount(uint64_t block_size) const;

    public:
        ThreadManager(const ThreadManager& rhs)             = delete;
        ThreadManager(ThreadManager&& rhs)                  = delete;
        ThreadManager()                                     = delete;
        ThreadManager operator = (const ThreadManager& rhs) = delete;
        ThreadManager operator = (ThreadManager&&)          = delete;

        ThreadManager(const utils::Path& input, const utils::Path& output, uint64_t block_size, uint32_t workers_count);
        ThreadManager(const utils::Path& input, const utils::Path& output, uint64_t block_size);

        [[nodiscard]] static uint32_t getOptimalWorkersCount();

        void run();
    };
}
