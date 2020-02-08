#include "task_queue.h"
#include "hash_maker.h"

namespace builder::threading
{
    void HashTask::perform()
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

    bool TaskQueue::tryPop(HashTask& task)
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

    void TaskQueue::push(const HashTask& task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_no_more_push)
        {
            m_queue.push(task);
            m_cv.notify_one();
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
