#pragma once

#include "utils.h"
#include "file_iterator.h"
#include "worker.h"
#include "progress_bar.h"

#include <thread>
#include <mutex>
#include <atomic>

namespace builder::threading
{
    class ThreadPool
    {
    private:
        const size_t                      m_workers_count;
        const size_t                      m_blocks_count;   
        
        std::vector<Worker>               m_workers;
        std::vector<std::thread>          m_threads;
        std::string                       m_signature_buffer;

        // shared between threads
        std::mutex                        m_file_iterator_mutex;
        std::mutex                        m_global_hashes_mutex;
        std::mutex                        m_console_mutex;
        filesys::FileIterator             m_file_iterator;
        std::vector<utils::BinaryBuffer>  m_hashes;

        // shared flags && couners
        std::atomic_bool                  m_stop;
        std::atomic_uint32_t              m_blocks_processed;

        uint32_t                          m_block_size;

        ProgressBar                       m_progress_bar;
        std::vector<std::thread>          m_progress_bar_thread;

        void initialize();
        void createWorkers();
        void createThreads();
        void joinAll();
        void flushResult();

        [[nodiscard]] uint32_t getBlocksCount(const utils::Path& filename, uint32_t block_size) const;

    public:
        ThreadPool(const ThreadPool& rhs)             = delete;
        ThreadPool(ThreadPool&& rhs)                  = delete;
        ThreadPool()                                  = delete;
        ThreadPool operator = (const ThreadPool& rhs) = delete;
        ThreadPool operator = (ThreadPool&&)          = delete;

        ThreadPool(const utils::Path& filename, uint32_t block_size, size_t workers_count);
        ThreadPool(const utils::Path& filename, uint32_t block_size);

        [[nodiscard]] static uint32_t getOptimalWorkersCount();
        [[nodiscard]] std::string getSignature() const;

        void run();
    };
}