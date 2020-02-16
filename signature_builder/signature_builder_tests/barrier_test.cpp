#include <gtest/gtest.h>

#include <barrier.h>
#include <atomic>

using namespace builder::threading;

#define SLAVES_COUNT 8

std::uint64_t  g_Counter{ 0 };
std::mutex     g_Console;
Barrier        g_Barrier{SLAVES_COUNT};

void Slave()
{
    while (true)
    {
        std::lock_guard<std::mutex> lock(g_Console);

        if (g_Counter % 5 == 0)
        {
            bool stop = g_Barrier.notifyMaster();

            if (stop)
            {
                std::cout << "WORKER [" << std::this_thread::get_id() << "] stop" << std::endl;
                return;
            }
        }

        std::cout << "WORKER [" << std::this_thread::get_id() << "] counter=" << g_Counter++ << std::endl;
    }
   
}

TEST(barrier, single)
{
    std::vector<std::thread> workers;

    for (int i = 0; i < SLAVES_COUNT; i++)
    {
        workers.emplace_back(Slave);
    }

    while (true)
    {
        g_Barrier.wait();
        std::cout << "[[MASTER WAKE UP]] counter=" << g_Counter++ << std::endl;

        if (g_Counter > 50)
        {
            g_Barrier.stop();
            g_Barrier.wakeSlaves();
            break;
        }

        g_Barrier.reset();
        g_Barrier.wakeSlaves();

    }

    for (auto& worker : workers)
    {
        worker.join();
    }

    std::cout << "MAIN STOP\n";

    EXPECT_EQ(1, 1);
}