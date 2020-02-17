#pragma once

#include <string>
#include <string_view>

#include <boost/program_options.hpp>

namespace builder::argparse
{
    namespace constants
    {
        constexpr std::string_view kParamHelp    { "help"    };
        constexpr std::string_view kParamWorkers { "workers" };
        constexpr std::string_view kParamInput   { "input"   };
        constexpr std::string_view kParamOutput  { "output"  };
        constexpr std::string_view kParamBlock   { "block"   };

        constexpr std::string_view kDescrHelp    { "help,h"    };
        constexpr std::string_view kDescrWorkers { "workers,w" };
        constexpr std::string_view kDescrInput   { "input,i"   };
        constexpr std::string_view kDescrOutput  { "output,o"  };
        constexpr std::string_view kDescrBlock   { "block,b"   };
    }

    namespace po = boost::program_options;

    class CommandLineManager
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

    class FileNotFound final
        : public std::exception
    {
    private:
        mutable std::string m_message;

    public:
        explicit FileNotFound(const std::string& filename);

        [[nodiscard]] const char *what() const noexcept override final
        {
            return m_message.c_str();
        }
    };

    class MissingEssentialOptions final
        : public std::exception
    {
    private:
        mutable std::string m_message;

    public:
        explicit MissingEssentialOptions(const po::options_description& description);

        [[nodiscard]] const char *what() const noexcept override final
        {
            return m_message.c_str();
        }
    };
}
