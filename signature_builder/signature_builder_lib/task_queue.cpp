#include "task_queue.h"
#include "hash_maker.h"

namespace builder::threading
{
    void Task::perform()
    {
        block.hash = crypto::HashMaker(block.buffer).getHash();
        block.buffer.reset();
    }

    TaskQueue::TaskQueue()
        : m_queue()
        , m_cv()
        , m_mutex()
        , m_no_more_push(false)
    {
    }

    bool TaskQueue::tryPop(Task& task)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_cv.wait(lock, [&]()
            {
                return m_no_more_push || !this->m_queue.empty();
            });

        if (m_no_more_push)
        {
            return false;
        }

        task = m_queue.front();
        m_queue.pop();

        return  true;
    }

    bool TaskQueue::tryPop(std::vector<Task>& tasks, uint64_t count)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_cv.wait(lock, [&]()
            {
                return m_no_more_push || !this->m_queue.empty();
            });

        if (m_no_more_push)
        {
            return false;
        }

        auto queue_size = m_queue.size();
        auto tasks_to_pop = queue_size > count
            ? count
            : queue_size;

        for (size_t i = 0; i < tasks_to_pop; i++)
        {
            tasks.emplace_back(m_queue.front());
            m_queue.pop();
        }

        return true;
    }

    void TaskQueue::push(const Task& task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_no_more_push)
        {
            m_queue.push(task);
            m_cv.notify_one();
        }
    }

    void TaskQueue::push(const std::vector<Task> &tasks)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        for (const auto& task : tasks)
        {
            m_queue.push(task);
        }
    }

    void TaskQueue::stop()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_no_more_push = true;
        m_cv.notify_all();
    }

    bool TaskQueue::stopped() 
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        return m_no_more_push;
    }

    TaskQueue::SizeType TaskQueue::size() const
    {
        return m_queue.size();
    }
}
