#pragma once

#include <vector>
#include <mutex>
#include <atomic>

#include "task_queue.h"

namespace builder::threading
{
    class Worker
    {
        
    private:
        TaskQueue&        m_hash_queue;
        TaskQueue&        m_flush_queue;
        std::vector<Task> m_tasks;
        std::mutex&       m_console_mutex;
        std::atomic_bool& m_stop_pool;

        static const uint64_t kMaxTasksPerIteration = 128;

    public:


        Worker
        (
            TaskQueue&        hash_queue,
            TaskQueue&        flush_queue,
            std::mutex&       console_mutex,
            std::atomic_bool& stop_pool
        )
            : m_hash_queue(hash_queue)
            , m_flush_queue(flush_queue)
            , m_tasks()
            , m_console_mutex(console_mutex)
            , m_stop_pool(stop_pool)
        {}

        void run();
    };
}
