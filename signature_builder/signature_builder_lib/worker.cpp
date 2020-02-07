#include "worker.h"
#include "hash_maker.h"
#include <iostream>

#define BLOCKS_COUNT_PER_READ 128


namespace builder::threading
{
    void Worker::readBlocks()
    {
        std::lock_guard<std::mutex> lock(m_file_iterator_mutex);

        m_blocks_processed_on_iteration = 0;
        m_local_blocks.clear();

        for (size_t i = 0; i < BLOCKS_COUNT_PER_READ; i++)
        {
            if (!m_file_iterator.moreDataAvailable())
            {
                m_stop_pool = true;
                return;
            }

            m_local_blocks.emplace_back(*++m_file_iterator);
        }
    }

    void Worker::calculateHashes()
    {
        m_local_hashes.clear();

        for (const auto& local_block : m_local_blocks)
        {
            m_local_hashes.emplace_back
            (
                filesys::BinaryBlock
                (
                    local_block.id,
                    crypto::HashMaker(local_block.buffer).getHash()
                )
            );

            m_blocks_processed_on_iteration++;
        }

        m_total_blocks_processed += m_blocks_processed_on_iteration;
    }

    void Worker::flushHashes()
    {
        std::lock_guard<std::mutex> lock(m_global_hashes_mutex);

        for (const auto& local_hash : m_local_hashes)
        {
            m_global_hashes[local_hash.id] = *local_hash.buffer;
        }
    }

    Worker::Worker
    (
        std::mutex&                        file_iterator_mutex,
        std::mutex&                        global_hashes_mutex,
        filesys::FileIterator&             file_iterator,
        std::vector<utils::BinaryBuffer>&  hashes,
        std::atomic_bool&                  stop_flag,
        std::atomic_uint32_t&              blocks_processed,
        std::mutex&                        console_mutex
    )
        : m_file_iterator_mutex(file_iterator_mutex)
        , m_global_hashes_mutex(global_hashes_mutex)
        , m_console_mutex(console_mutex)
        , m_file_iterator(file_iterator)
        , m_global_hashes(hashes)
        , m_stop_pool(stop_flag)
        , m_total_blocks_processed(blocks_processed)
        , m_blocks_processed_on_iteration(0)
    {
    }

    void Worker::run() try
    {
        while (!m_stop_pool)
        {
            readBlocks();
            calculateHashes();
            flushHashes();
        }
    }
    catch (const std::exception& ex)
    {
        std::lock_guard<std::mutex> lock(m_console_mutex);
        std::cout << "thread - " <<  std::this_thread::get_id() << " failed with - " << ex.what() << std::endl;
    }
}
