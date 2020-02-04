#include "thread_pool.h"

namespace builder::threading
{
    void ThreadPool::initialize()
    {
        m_workers.reserve(m_workers_count);
        m_hashes->reserve(m_blocks_count);
    }

    void ThreadPool::createWorkers()
    {
        for (size_t i = 0; i < m_workers_count; i++)
        {
            m_workers.emplace_back
            (
                std::thread{ m_file_iterator_mutex, m_file_iterator, m_hashes }
            );
        }
    }

    void ThreadPool::joinAll()
    {
        for (auto& worker : m_workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    void ThreadPool::flushResult()
    {
        std::string buffer_to_flush;
        buffer_to_flush.reserve(64 * m_hashes->size()); // TODO: Заменить 64 на дефайн хотя-бы

        for (const auto& hash : *m_hashes)
        {
            buffer_to_flush += hash;
        }

        // TODO: Записать в файл
    }

    size_t ThreadPool::getOptimalWorkersCount()
    {
        return std::thread::hardware_concurrency() + 1;
    }

    size_t ThreadPool::getBlocksCount(const utils::Path &filename, size_t block_size) const
    {
        auto size = utils::fs::file_size(filename);

        return (size % block_size) ? (size / block_size) : (size / block_size + 1);
    }

    ThreadPool::ThreadPool(const utils::Path &filename, size_t block_size, size_t workers_count)
        : m_workers_count(workers_count)
        , m_blocks_count(getBlocksCount(filename, block_size))
        , m_file_iterator(std::make_shared<filesys::FileIterator>(filename, block_size))
    {
        initialize();
    }

    ThreadPool::ThreadPool(const utils::Path &filename, size_t block_size)
        : m_workers_count(getOptimalWorkersCount())
        , m_blocks_count(getBlocksCount(filename, block_size))
        , m_file_iterator(std::make_shared<filesys::FileIterator>(filename, block_size))
    {
        initialize();
    }

    void ThreadPool::run()
    {
        createWorkers();
        joinAll();
        flushResult();
    }
}
