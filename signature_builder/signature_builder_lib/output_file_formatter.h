#pragma once

#include "utils.h"

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

        void checkFileOpening() const;
        void fillWithZeros();

    public:
        OutputFileFormatter(const utils::Path& file_name, uint64_t hash_blocks_count);
        OutputFileFormatter() = default;
    };

    class FileNotOpened final
        : public std::exception
    {
    public:
        [[nodiscard]] const char *what() const noexcept override final;
    };
}
