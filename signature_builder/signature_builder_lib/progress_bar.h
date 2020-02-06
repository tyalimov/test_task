#pragma once
#include <atomic>
#include <mutex>

namespace builder::threading
{
    class ProgressBar
    {
    private:
        std::atomic_bool&       m_stop_pool;
        std::atomic_uint32_t&   m_total_blocks_processed;
        uint32_t                m_total_blocks;
        std::mutex&             m_console_mutex;

    public:
        ProgressBar
        (
            std::atomic_uint32_t& blocks_processed,
            std::atomic_bool&     stop,
            uint32_t              total_blocks,
            std::mutex&           console_mutex
        );

        void run();
    };
}
