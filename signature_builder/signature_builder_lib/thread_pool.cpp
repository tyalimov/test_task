#include "thread_pool.h"
#include "worker_thread.h"

namespace builder::threading
{
    void ThreadPool::initialize()
    {
        m_workers.reserve(m_workers_count);
        m_threads.reserve(m_workers_count);
        m_hashes = std::vector<utils::BinaryBuffer>{ m_blocks_count, utils::BinaryBuffer() };
        m_signature_buffer.reserve(SHA512_DIGEST_SIZE * m_hashes.size());
    }

    void ThreadPool::createWorkers()
    {
        for (size_t i = 0; i < m_workers_count; i++)
        {
            m_workers.emplace_back
            (
                Worker
                {
                    m_file_iterator_mutex,
                    m_global_hashes_mutex,
                    m_file_iterator,
                    m_hashes,
                    m_stop
                }
            );
        }
    }

    void ThreadPool::createThreads()
    {
        for (size_t i = 0; i < m_workers_count; i++)
        {
            m_threads.emplace_back
            (
                std::thread{ &Worker::run, &m_workers[i] }
            );
        }
    }

    void ThreadPool::joinAll()
    {
        for (auto& thread : m_threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }

    void ThreadPool::flushResult()
    {
        for (auto hash =  m_hashes.begin(); hash != m_hashes.end();)
        {
            m_signature_buffer += std::string{hash->begin(), hash->end()};
            hash = m_hashes.erase(hash);
        }
    }

    size_t ThreadPool::getOptimalWorkersCount()
    {
        return std::thread::hardware_concurrency() + 1;
    }

    std::string ThreadPool::getSignature() const
    {
        return m_signature_buffer;
    }

    size_t ThreadPool::getBlocksCount(const utils::Path &filename, size_t block_size) const
    {
        auto size = utils::fs::file_size(filename);

        return (size % block_size) ? (size / block_size + 1) : (size / block_size);
    }

    ThreadPool::ThreadPool(const utils::Path &filename, size_t block_size, size_t workers_count)
        : m_workers_count(workers_count)
        , m_blocks_count(getBlocksCount(filename, block_size))
        , m_file_iterator(filename, block_size)
        , m_stop(false)
    {
        initialize();
    }

    ThreadPool::ThreadPool(const utils::Path &filename, size_t block_size)
        : m_workers_count(getOptimalWorkersCount())
        , m_blocks_count(getBlocksCount(filename, block_size))
        , m_file_iterator(filename, block_size)
        , m_stop(false)
    {
        initialize();
        createWorkers();
    }

    void ThreadPool::run()
    {
        createWorkers();
        createThreads();
        joinAll();
        flushResult();
    }
}
