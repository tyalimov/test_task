#pragma once

#include "utils.h"
#include "file_mapper.h"
#include "worker.h"

#include <thread>
#include <mutex>


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

#ifdef _DEBUG
#   define LOG_MAIN(msg)                                    \
    {                                                       \
        std::lock_guard<std::mutex> lock(m_console_mutex);  \
        std::cout << "Main - ";                             \
        std::cout << msg << std::endl;                      \
    }
#else
#   define LOG(msg)
#endif

namespace builder::threading
{
    class ThreadManager
    {
    private:
        std::vector<Worker>      m_workers;
        std::vector<std::thread> m_threads;

        filesys::FileMapper m_input_mapper;
        filesys::FileMapper m_output_mapper;

        uint32_t m_workers_count;
        
        uint64_t m_total_blocks;
        uint64_t m_current_limit;
        uint64_t m_current_block_id;

        Barrier    m_barrier;
        std::mutex m_block_id_mutex;
        std::mutex m_console_mutex;

        void mapFiles();
        void createWorkers();
        void createThreads();
        void joinAll();

        // task management
        void updateProgressBar() const;

#ifdef _DEBUG
        void printStats() const;
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
