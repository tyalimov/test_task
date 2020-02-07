#include "progress_bar.h"

#include <iostream>
#include <boost/format.hpp>
#include <thread>

namespace builder::threading
{
    ProgressBar::ProgressBar
    (
        std::atomic_uint32_t& blocks_processed, 
        std::atomic_bool&     stop, 
        uint32_t              total_blocks,
        std::mutex&           console_mutex
    )
        : m_stop_pool(stop)
        , m_total_blocks_processed(blocks_processed)
        , m_total_blocks(total_blocks)
        , m_console_mutex(console_mutex)
    {
    }

    void ProgressBar::run() try
    {
        while (!m_stop_pool)
        {
            std::lock_guard<std::mutex> lock(m_console_mutex);

            uint32_t processed  = m_total_blocks_processed;
            double   percentage = (double(processed) / double(m_total_blocks)) * 100.0;

            std::cout << boost::format("\rblocks processed: %d/%d | %d%%       ")
                % processed
                % m_total_blocks
                % percentage;

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        std::cout << boost::format("\rblocks processed: %d/%d | %d%%       ")
        % m_total_blocks
        % m_total_blocks
        % 100.0;
    }
    catch (const std::exception& ex)
    {
        std::lock_guard<std::mutex> lock(m_console_mutex);
        std::cout << "thread - " <<  std::this_thread::get_id() << " failed with - " << ex.what() << std::endl;
    }
}
