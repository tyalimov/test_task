#include "barrier.h"

namespace builder::threading
{
    Barrier::Barrier(uint32_t slaves_count)
        : m_slaves_mtx()
        , m_slaves_cv()
        , m_master_mtx()
        , m_master_cv()
        , m_stop_slaves(false)
        , m_slaves_count(slaves_count)
        , m_notifications_count(0)
    {
    }

    // true  - stop
    // false - continue
    bool Barrier::notifyMaster()
    {
        if (m_stop_slaves)
        {
            return false;
        }

        if (++m_notifications_count == m_slaves_count)
        {
            m_master_cv.notify_all();
            std::unique_lock<std::mutex> lock(m_slaves_mtx);
            m_slaves_cv.wait(lock);
            return m_stop_slaves;
        }
        
        std::unique_lock<std::mutex> lock(m_slaves_mtx);
        m_slaves_cv.wait(lock);
        return m_stop_slaves;
    }

    void Barrier::reset()
    {
        m_notifications_count = 0;
    }

    void Barrier::wait()
    {
        std::unique_lock<std::mutex> lock(m_master_mtx);
        m_master_cv.wait(lock);
    }

    void Barrier::wakeSlaves()
    {
        m_slaves_cv.notify_all();
    }

    void Barrier::stop()
    {
        m_stop_slaves = true;
    }
}
