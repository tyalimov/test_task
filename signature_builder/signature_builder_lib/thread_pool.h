#pragma once

#include "utils.h"
#include "file_iterator.h"

#include <thread>
#include <mutex>

// TODO: выбери уже между chunk и block

namespace builder::threading
{
    class ThreadPool
    {
    private:
        const size_t                               m_workers_count;
        const size_t                               m_blocks_count;   
        std::vector<std::thread>                   m_workers;
        std::shared_ptr<std::mutex>                m_file_iterator_mutex;
        std::shared_ptr<filesys::FileIterator>     m_file_iterator;
        std::shared_ptr<std::vector<utils::BinaryBufferPtr>>  m_hashes;

        void initialize();
        void createWorkers();
        void joinAll();
        void flushResult();


        [[nodiscard]] static size_t getOptimalWorkersCount();
        [[nodiscard]] size_t getBlocksCount(const utils::Path& filename, size_t block_size) const;

    public:
        ThreadPool(const ThreadPool& rhs)             = delete;
        ThreadPool(ThreadPool&& rhs)                  = delete;
        ThreadPool()                                  = delete;
        ThreadPool operator = (const ThreadPool& rhs) = delete;
        ThreadPool operator = (ThreadPool&&)          = delete;

        ThreadPool(const utils::Path& filename, size_t block_size, size_t workers_count);
        ThreadPool(const utils::Path& filename, size_t block_size);

        void run();
    };
}