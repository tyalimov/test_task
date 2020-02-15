#pragma once

#include <string>
#include <boost/program_options.hpp>

#define PARAM_HELP     "help"
#define PARAM_WORKERS  "workers"
#define PARAM_INPUT    "input"
#define PARAM_OUTPUT   "output"
#define PARAM_BLOCK    "block"

#define SHORT_HELP     ",h"
#define SHORT_WORKERS  ",w"
#define SHORT_INPUT    ",i"
#define SHORT_OUTPUT   ",o"
#define SHORT_BLOCK    ",b"

#define DESCR_HELP     PARAM_HELP    SHORT_HELP
#define DESCR_WORKERS  PARAM_WORKERS SHORT_WORKERS
#define DESCR_INPUT    PARAM_INPUT   SHORT_INPUT
#define DESCR_OUTPUT   PARAM_OUTPUT  SHORT_OUTPUT
#define DESCR_BLOCK    PARAM_BLOCK   SHORT_BLOCK

#define DEFAULT_BLOCK_SIZE (1024 * 1024)

namespace builder::argparse
{
    namespace po = boost::program_options;

    struct CommandLineManager
    {
    public:
        po::options_description description;
        void verifyParameters() const;
        void prepareParameters() const;

    private:
        po::variables_map       variables_map;

        int                     args_count;
        const char              **args_list;

        void readParameters();
        void setFlags();
        void assignValues();

    public:
        CommandLineManager(int argc, const char **argv);

        std::string input_file;
        std::string output_file;
        uint32_t    workers_count;
        uint64_t    block_size;

        bool        custom_workers_count_specified;
        bool        required_parameters_specified;
        bool        custom_block_size_specified;
    };
}
