#include <iostream>
#include <fstream>
#include <chrono>

#include <thread_pool.h>
#include <boost/format.hpp>
#include "cmd_manager.h"

int main(int argc, const char *argv[]) try
{
    builder::argparse::CommandLineManager cmd{ argc, argv };

    if (!cmd.required_parameters_specified)
    {
        std::cout << cmd.description << std::endl;
        return 0;
    }

    cmd.verifyParameters();
    std::cout << "[SUCCESS] - Verifying parameters\n";

    cmd.prepareParameters();
    std::cout << "[SUCCESS] - Getting ready for execution\n\n";

    std::cout
        << boost::format("input file ---- [%s]\noutput file --- [%s]\nblock size ---- [%d bytes]\nthreads count - [%d]\n")
        % cmd.input_file
        % cmd.output_file
        % cmd.block_size
        % cmd.workers_count;


    auto start = std::chrono::high_resolution_clock::now();

    builder::threading::ThreadPool pool{ cmd.input_file, cmd.block_size, cmd.workers_count };
    pool.run();

    std::ofstream out( cmd.output_file, std::ios::out | std::ios::binary );
    out << pool.getSignature();

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << 
        boost::format("\nSignature generation finished.\ncalculation time - [%d seconds]\n")
        % std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
}
catch (const std::exception& ex)
{
    std::cout << "[FAILURE] - " << ex.what() << std::endl;
}
catch (...)
{
     std::cout << "[FAILURE] - " << "Unexpected error\n";
}
