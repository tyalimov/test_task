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
