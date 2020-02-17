#pragma once

#include <misc/utils.h>

#include <fstream>

namespace builder::filesys
{
    namespace constants
    {
        enum Constants
            : uint64_t
        {
            kFillingBlockSize = 15 * 1024 * 1024
        };
    }
    
    class OutputFileFormatter
    {
    private:
        static const char kDataToFill[constants::kFillingBlockSize];
        
        std::ofstream     m_file;
        uint64_t          m_file_size;
        utils::Path       m_file_name;

        void checkFileOpening() const;
        void fillWithZeros();

    public:
        OutputFileFormatter(const utils::Path& file_name, uint64_t hash_blocks_count);
        OutputFileFormatter() = default;
    };

    class FileNotOpened final
        : public std::exception
    {
    private:
        mutable std::string m_message;

    public:
        explicit FileNotOpened(const std::string& filename);

        [[nodiscard]] const char *what() const noexcept override final;
    };
}
