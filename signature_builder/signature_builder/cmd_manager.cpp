#include "cmd_manager.h"
#include "thread_pool.h"

#include <utils.h>
#include <boost/format.hpp>

namespace builder::argparse
{
    void CommandLineManager::readParameters()
    {
        description.add_options()
            (DESCR_HELP    , "Print this message")
            (DESCR_INPUT   , po::value<std::string>(&input_file) , "Path to input file to calculate signature")
            (DESCR_OUTPUT  , po::value<std::string>(&output_file), "Path to output file to store signature")
            (DESCR_BLOCK   , po::value<size_t>(&block_size)      , "Block size to read from file (optional)")
            (DESCR_WORKERS , po::value<size_t>(&workers_count)   , "Maximum number of working threads (optional)")
            ;
        po::parsed_options parsed_options = po::command_line_parser(args_count, args_list).options(description).run();
        po::store(parsed_options, variables_map);
        po::notify(variables_map);
    }

    void CommandLineManager::setFlags()
    {
        custom_workers_count_specified 
            = (variables_map.count(PARAM_WORKERS) != 0);

        required_parameters_specified
            =  (variables_map.count(PARAM_INPUT) != 0)
            && (variables_map.count(PARAM_OUTPUT) != 0);

        custom_block_size_specified
            = (variables_map.count(PARAM_BLOCK) != 0);
    }

    void CommandLineManager::assignValues()
    {
        workers_count = custom_workers_count_specified
            ? variables_map[PARAM_WORKERS].as<size_t>()
            : threading::ThreadPool::getOptimalWorkersCount();

        block_size = custom_block_size_specified
            ? variables_map[PARAM_BLOCK].as<size_t>()
            : DEFAULT_BLOCK_SIZE;
    }

    void CommandLineManager::verifyParameters() const
    {
        if (!utils::fs::exists(input_file))
        {
            throw std::runtime_error((boost::format("File [%s] does not exist") % input_file).str().c_str());
        }
    }

    void CommandLineManager::prepareParameters() const
    {
        utils::Path dir_path{ output_file };
        dir_path.remove_filename();
        utils::fs::create_directories(dir_path);
    }

    CommandLineManager::CommandLineManager(int argc, const char **argv)
        : description("Signature builder tool")
        , args_count(argc)
        , args_list(argv)
    {
        readParameters();
        setFlags();
        assignValues();
    }
}
