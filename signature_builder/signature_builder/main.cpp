#include "cmd_manager.h"
#include "utils.h"

#include <file_hash_builder.h>

#include <iostream>
#include <chrono>

#include <boost/format.hpp>

int main(int argc, const char *argv[]) try
{
    builder::argparse::CommandLineManager cmd{ argc, argv };

    std::cout << "Verifying parameters......... ";
    cmd.verifyParameters();
    std::cout << "[SUCCESS]\n";

    std::cout << "Configuring execution........ ";
    cmd.prepareParameters();
    std::cout << "[SUCCESS]\n";

    std::cout << "Creating signature storage... ";
    builder::utils::CreateOutputFile(cmd.input_file, cmd.output_file, cmd.block_size);
    std::cout << "[SUCCESS]\n\n";

    std::cout << boost::format("input file ---- [%s]\n"
                               "output file --- [%s]\n"
                               "block size ---- [%d bytes]\n"
                               "threads count - [%d]\n")
        % cmd.input_file
        % cmd.output_file
        % cmd.block_size
        % cmd.workers_count;

    std::cout << "\n###### Calculating signature ######\n\n";

    auto start = std::chrono::high_resolution_clock::now();

    builder::threading::FileHashBuilder 
    { 
        cmd.input_file, 
        cmd.output_file, 
        cmd.block_size, 
        cmd.workers_count 
    }.run();

    auto end = std::chrono::high_resolution_clock::now();

    auto min_passed = std::chrono::duration_cast<std::chrono::minutes>(end - start).count();
    auto sec_passed = std::chrono::duration_cast<std::chrono::seconds>(end - start).count() % 60;
    auto ms_passed  = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() % 1000;

    std::cout << boost::format("\nSignature generation finished.\n"
                               "Calculation time (MM:SS:MMM) - [%d:%d:%d]\n")
        % min_passed
        % sec_passed
        % ms_passed;
}
catch (const builder::argparse::MissingEssentialOptions& ex)
{
    std::cout << "\r                                        \r" 
        << ex.what() << std::endl;
}
catch (const std::exception& ex)
{
    std::cout << "\nFAILURE: " << ex.what() << std::endl;
}
catch (...)
{
     std::cout << "\nFAILURE: " << "Unexpected error\n";
}