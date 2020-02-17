#include "cmd_manager.h"

#include <threading/file_hash_builder.h>
#include <misc/utils.h>

#include <boost/format.hpp>

namespace builder::argparse
{
    void CommandLineManager::readParameters()
    {
        description.add_options()
            (constants::kDescrHelp.data()    , "Print this message")
            (constants::kDescrInput.data()   , po::value<std::string>(&input_file) , "Path to input file to calculate signature")
            (constants::kDescrOutput.data()  , po::value<std::string>(&output_file), "Path to output file to store signature")
            (constants::kDescrBlock.data()   , po::value<uint64_t>(&block_size)    , "Block size to read from file (optional)")
            (constants::kDescrWorkers.data() , po::value<uint32_t>(&workers_count) , "Maximum number of working threads (optional)");

        po::parsed_options parsed_options = po::command_line_parser(args_count, args_list).options(description).run();
        po::store(parsed_options, variables_map);
        po::notify(variables_map);
    }

    void CommandLineManager::setFlags()
    {
        custom_workers_count_specified 
            = variables_map.count(constants::kParamWorkers.data()) != 0;

        required_parameters_specified
            =  variables_map.count(constants::kParamInput.data()) != 0 && 
               variables_map.count(constants::kParamOutput.data()) != 0;

        custom_block_size_specified
            = variables_map.count(constants::kParamBlock.data()) != 0;
    }

    void CommandLineManager::assignValues()
    {
        workers_count = custom_workers_count_specified
            ? variables_map[constants::kParamWorkers.data()].as<uint32_t>()
            : threading::FileHashBuilder::getOptimalWorkersCount();

        block_size = custom_block_size_specified
            ? variables_map[constants::kParamBlock.data()].as<uint64_t>()
            : utils::constants::kDefaultBlockSize;
    }

    void CommandLineManager::verifyParameters() const
    {
        if (!required_parameters_specified)
        {
            throw MissingEssentialOptions(description);
        }

        if (!utils::fs::exists(input_file))
        {
            throw FileNotFound(input_file);
        }
    }

    void CommandLineManager::prepareParameters() const
    {
        utils::fs::create_directories(utils::Path{ output_file }.remove_filename());
    }

    CommandLineManager::CommandLineManager(int argc, const char **argv)
        : description("Signature builder tool")
        , args_count(argc)
        , args_list(argv)
        , input_file()
        , output_file()
        , workers_count(0)
        , block_size(utils::constants::kDefaultBlockSize)
        , custom_workers_count_specified(false)
        , required_parameters_specified(false)
        , custom_block_size_specified(false)
    {
        readParameters();
        setFlags();
        assignValues();
    }

    FileNotFound::FileNotFound(const std::string& filename)
        : m_message("Cannot find the path specified - [")
    {
        m_message += filename;
        m_message += "]";
    }

    const char *FileNotFound::what() const noexcept
    {
        return m_message.c_str();
    }

    MissingEssentialOptions::MissingEssentialOptions(const po::options_description& description)
        : m_message()
    {
        std::stringstream ss;
        ss << description;

        m_message = ss.str();
    }

    const char *MissingEssentialOptions::what() const noexcept
    {
        return m_message.c_str();
    }
}
