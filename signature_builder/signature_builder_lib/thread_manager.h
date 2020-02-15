#pragma once

#include "utils.h"
#include "file_mapper.h"
#include "worker.h"

#include <thread>
#include <mutex>
#include <atomic>


#ifdef _DEBUG
#   define DBG_PRINT_STATS() { printStats(); }
#else
#   define DBG_PRINT_STATS()
#endif

#ifndef _DEBUG
#   define UPDATE_PROGRESS_BAR() { updateProgressBar(); }
#else
#   define UPDATE_PROGRESS_BAR()
#endif

namespace builder::threading
{
    class ThreadManager
    {
    private:

        std::vector<Worker>      m_workers;
        std::vector<std::thread> m_threads;
        std::mutex               m_console_mutex;
        std::atomic_bool         m_stop_pool;

        filesys::FileMapper      m_input_mapper;
        filesys::FileMapper      m_output_mapper;

        uint32_t                 m_workers_count;
        uint64_t                 m_total_blocks;

        std::atomic_uint64_t     m_current_block_id;

        void mapFiles();
        void createWorkers();
        void createThreads();
        void joinAll();

        // task management
        void updateProgressBar() const;

#ifdef _DEBUG
        void printStats();
#endif

        [[nodiscard]] static uint64_t getBlocksCount(const utils::Path &input, uint64_t block_size);

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
