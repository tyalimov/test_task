#pragma once

#include <mutex>
#include <condition_variable>

namespace builder::threading
{
    class Barrier
    {
    private:
        std::mutex              m_slaves_mtx;
        std::condition_variable m_slaves_cv;

        std::mutex              m_master_mtx;
        std::condition_variable m_master_cv;

        bool     m_stop_slaves;
        uint32_t m_slaves_count;
        uint32_t m_notifications_count;

        //uint64_t 

    public:
        explicit Barrier(uint32_t slaves_count);

        Barrier() = delete;

        // Called by slave
        bool notifyMaster();

        // Called by master
        void reset();
        void wait();
        void wakeSlaves();
        void stop();
    };
}