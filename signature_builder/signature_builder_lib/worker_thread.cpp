#include "worker_thread.h"
#include "hash_maker.h"
#include <iostream>

#define BLOCKS_COUNT_PER_READ 64

//#define THREADING_DEBUG

#ifdef THREADING_DEBUG
#   include <iostream>
#   include <boost/format.hpp>
#endif

std::mutex g_Cons;

namespace builder::threading
{
    // TODO: Отдельно обработать 0 байт
    void Worker::readBlocks()
    {
        std::lock_guard<std::mutex> lock(m_file_iterator_mutex);

        for (size_t i = 0; i < BLOCKS_COUNT_PER_READ; i++)
        {
#ifdef THREADING_DEBUG
            std::cout <<
            boost::format("-------- [worker %d] reading block=%d\n")
                % std::this_thread::get_id()
                % i;
#endif

            if (!m_file_iterator.moreDataAvailable())
            {
#ifdef THREADING_DEBUG
                std::cout <<
                boost::format("-------- [worker %d] no more data available\n")
                    % std::this_thread::get_id();
#endif
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
#ifdef THREADING_DEBUG
            std::cout <<
            boost::format("-------- [worker %d] hash for block=%d\n")
                % std::this_thread::get_id()
                % local_block.id;
#endif
            m_local_hashes.emplace_back
            (
                filesys::BinaryBlock
                (
                    local_block.id,
                    crypto::HashMaker(local_block.buffer).getHash()
                )
            );
        }
    }

    void Worker::flushHashes()
    {
        std::lock_guard<std::mutex> lock(m_global_hashes_mutex);

        for (const auto& local_hash : m_local_hashes)
        {
#ifdef THREADING_DEBUG
            std::cout <<
                boost::format("-------- [worker %d] flushing hash=%d\n")
                    % std::this_thread::get_id()
                    % local_hash.id;
#endif
            m_global_hashes[local_hash.id] = *local_hash.buffer;
        }
    }

    Worker::Worker
    (
        std::mutex&                        file_iterator_mutex,
        std::mutex&                        global_hashes_mutex,
        filesys::FileIterator&             file_iterator,
        std::vector<utils::BinaryBuffer>&  hashes,
        std::atomic_bool&                  stop_flag
    )
        : m_file_iterator_mutex(file_iterator_mutex)
        , m_global_hashes_mutex(global_hashes_mutex)
        , m_file_iterator(file_iterator)
        , m_global_hashes(hashes)
        , m_stop_pool(stop_flag)
        , m_readed_blocks_on_current_iteration(0)
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
        std::lock_guard<std::mutex> lock(g_Cons);
        std::cout << "thread - " <<  std::this_thread::get_id() << " failed - " << ex.what() << std::endl;
    }
}
