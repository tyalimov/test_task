﻿#pragma once

#include "file_iterator.h"

#include <vector>
#include <mutex>
#include <atomic>

namespace builder::threading
{
    class Worker
    {
    private:
        std::mutex&                        m_file_iterator_mutex; 
        std::mutex&                        m_global_hashes_mutex;
        std::mutex&                        m_console_mutex;
        filesys::FileIterator&             m_file_iterator;
        std::vector<utils::BinaryBuffer>&  m_global_hashes;

        std::atomic_bool&                  m_stop_pool;
        std::atomic_uint32_t&              m_total_blocks_processed;

        uint32_t m_blocks_processed_on_iteration;

        void readBlocks();
        void calculateHashes();
        void flushHashes();

    public:
        Worker
        (
            std::mutex&                        file_iterator_mutex,
            std::mutex&                        global_hashes_mutex,
            filesys::FileIterator&             file_iterator,
            std::vector<utils::BinaryBuffer>&  hashes,
            std::atomic_bool&                  stop_flag,
            std::atomic_uint32_t&              blocks_processed,
            std::mutex&                        console_mutex
        );

        std::vector<filesys::BinaryBlock> m_local_blocks;
        std::vector<filesys::BinaryBlock> m_local_hashes;

        void run();
    };
}