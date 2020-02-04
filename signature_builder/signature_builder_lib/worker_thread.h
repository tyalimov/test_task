﻿#pragma once

#include "file_iterator.h"

#include <string>
#include <vector>
#include <memory>
#include <mutex>

namespace builder::threading
{
    // TODO: Может быть передать сюда block_size, чтобы std::string могла проаллоцировать память?
    class WorkerThread
    {
    private:
        std::vector<std::string>                   m_blocks;
        std::shared_ptr<std::mutex>                m_file_iterator_mutex;
        std::shared_ptr<filesys::FileIterator>     m_file_iterator;
        std::shared_ptr<std::vector<std::string>>  m_hashes;

    public:
        WorkerThread
        (
            const std::shared_ptr<std::mutex>&               file_iterator_mutex,
            const std::shared_ptr<filesys::FileIterator>&    file_iterator,
            const std::shared_ptr<std::vector<std::string>>& hashes
        );

        void run();
    };

    void WorkerThreadFunction
    (
        const std::shared_ptr<std::mutex>&               file_iterator_mutex,
        const std::shared_ptr<filesys::FileIterator>&    file_iterator,
        const std::shared_ptr<std::vector<std::string>>& hashes
    );
}
