#include "worker.h"
#include "hash_maker.h"

#include <iostream>
#include <boost/format.hpp>

#define THREAD_DEBUG

namespace builder::threading
{
    
    void Worker::run() try
    {
#ifdef _DEBUG
        {
            std::lock_guard<std::mutex> lock(m_console_mutex);
            std::cout << boost::format("thread [%d] started\n")
                % std::this_thread::get_id();
        }
#endif

        while (!m_stop_pool)
        {
            m_tasks.clear();

            auto status = m_hash_queue.tryPop(m_tasks, kMaxTasksPerIteration);

            if (!status)
            {
#ifdef _DEBUG
                {
                    std::lock_guard<std::mutex> lock(m_console_mutex);
                    std::cout << boost::format("thread [%d] finished because of no more push\n")
                        % std::this_thread::get_id();
                }
#endif
                break;
            }

#ifdef _DEBUG
                {
                    std::lock_guard<std::mutex> lock(m_console_mutex);
                    std::cout << boost::format("thread [%d] got %d tasks\n")
                        % std::this_thread::get_id()
                        % m_tasks.size();
                }
#endif

            for (auto& task : m_tasks)
            {
                task.perform();
            }
            
            m_flush_queue.push(m_tasks);
        }
    }
    catch (const std::exception& ex)
    {
        std::lock_guard<std::mutex> lock(m_console_mutex);

        std::cout << boost::format("thread [%d] failed - [%s]")
            % std::this_thread::get_id()
            % ex.what();
    }
}
