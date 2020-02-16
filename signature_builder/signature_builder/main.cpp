#if 1
#include <iostream>
#include <chrono>

#include <thread_manager.h>
#include <boost/format.hpp>

#include "cmd_manager.h"
#include "utils.h"

// Скорее всего много времени уходит на flush.
// Надо пробовать хешировать данные огромными кусками
// И попробовать убрать flush, вдруг это уберет нагрузку с диска
// Время считается через жопу

int main(int argc, const char *argv[]) try
{
    builder::argparse::CommandLineManager cmd{ argc, argv };

    if (!cmd.required_parameters_specified)
    {
        std::cout << cmd.description << std::endl;
        return 0;
    }

    std::cout << "Verifying parameters......... ";
    cmd.verifyParameters();
    std::cout << "[SUCCESS]\n";

    std::cout << "Configuring execution........ ";
    cmd.prepareParameters();
    std::cout << "[SUCCESS]\n";

    std::cout << "Creating signature storage... ";
    builder::utils::CreateOutputFile(cmd.input_file, cmd.output_file, cmd.block_size);
    std::cout << "[SUCCESS]\n\n";

    std::cout
        << boost::format("input file ---- [%s]\noutput file --- [%s]\nblock size ---- [%d bytes]\nthreads count - [%d]\n")
        % cmd.input_file
        % cmd.output_file
        % cmd.block_size
        % cmd.workers_count;

    std::cout << "\n###### Calculating signature ######\n\n";

    auto start = std::chrono::high_resolution_clock::now();

    builder::threading::ThreadManager { cmd.input_file, cmd.output_file, cmd.block_size, cmd.workers_count }.run();

    auto end = std::chrono::high_resolution_clock::now();

    auto min_passed = std::chrono::duration_cast<std::chrono::minutes>(end - start).count();
    auto sec_passed = std::chrono::duration_cast<std::chrono::seconds>(end - start).count() % 60;
    auto ms_passed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() % 1000;

    std::cout <<
        boost::format("\nSignature generation finished.\ncalculation time (MM:SS:MMM) - [%d:%d:%d]\n")
        % min_passed
        % sec_passed
        % ms_passed;
}
catch (const std::exception& ex)
{
    std::cout << "[FAILURE] - " << ex.what() << std::endl;
}
catch (...)
{
     std::cout << "[FAILURE] - " << "Unexpected error\n";
}
#endif


#if 0
#include <barrier.h>
#include <iostream>
#include <vector>
#include <tuple>

using namespace builder::threading;

#define SLAVES_COUNT 8

uint64_t       g_Counter{ 1 };
uint64_t       g_Limit   { 0 };
std::mutex     g_Console;
std::mutex     g_CounterMtx;
Barrier        g_Barrier{SLAVES_COUNT};

std::tuple<bool, uint64_t> GetCounter()
{
    std::lock_guard<std::mutex> lock(g_CounterMtx);

    if (g_Counter < g_Limit)
    {
        return std::make_tuple(true, g_Counter++);
    }

    return std::make_tuple(false, uint64_t(0));
}

void Slave()
{
    {
        std::lock_guard<std::mutex> lock(g_Console);
        std::cout << "WORKER [" << std::this_thread::get_id() << "] start" << std::endl;
    }

    while (true)
    {
        auto [status, counter] = GetCounter();

        if (!status)
        {
            {
                std::lock_guard<std::mutex> lock(g_Console);
                std::cout << "WORKER [" << std::this_thread::get_id() << "] notify master" << std::endl;
            }

            bool stop = g_Barrier.notifyMaster();

            if (stop)
            {
                {
                    std::lock_guard<std::mutex> lock(g_Console);
                    std::cout << "WORKER [" << std::this_thread::get_id() << "] stop" << std::endl;
                }
                return;
            }
            else
            {
                continue;
            }
        }
        else
        {
            std::lock_guard<std::mutex> lock(g_Console);
            std::cout << "WORKER [" << std::this_thread::get_id() << "] counter=" << counter << std::endl;
        }
    }
   
}


int main()
{
    std::vector<std::thread> workers;

    std::cout << "[[MASTER START]]" << std::endl;

    g_Limit = 5;

    {
        std::lock_guard<std::mutex> lock(g_Console);
        for (int i = 0; i < SLAVES_COUNT; i++)
        {
            std::cout << "[[MASTER ADDING WORKER]]" << std::endl;
            workers.emplace_back(Slave);
        }
    }

    while (true)
    {
        g_Barrier.wait();

        {
            std::lock_guard<std::mutex> lock(g_Console);
            std::cout << "[[MASTER WAKE UP]] counter=" << g_Counter << std::endl;
        }

        if (g_Counter > 50)
        {
            {
                std::lock_guard<std::mutex> lock(g_Console);
                std::cout << "[[MASTER STOP WORKERS]]" << std::endl;
            }
            g_Barrier.stop();
            g_Barrier.wakeSlaves();
            break;
        }

        {
                std::lock_guard<std::mutex> lock(g_Console);
                std::cout << "[[MASTER START WORKERS]]" << std::endl;
                g_Limit += 5;
        }

        g_Barrier.reset();
        g_Barrier.wakeSlaves();

    }

    for (auto& worker : workers)
    {
        worker.join();
    }

    std::cout << "MAIN STOP\n";

    return 0;
}
#endif