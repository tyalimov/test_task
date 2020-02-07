#pragma once

#include "utils.h"

#include <fstream>

#define ZERO_BLOCK_SIZE (15 * 1024 * 1024)

namespace builder::filesys
{
    // TODO: file.exceptions ( ofstream::badbit );
    // TODO: Если файл уже существует, слать в жопу
    class ResultWriter
    {
    private:
        static const char kDataToFill[ZERO_BLOCK_SIZE];
        
        std::ofstream     m_file;
        uint64_t          m_file_size;

        void checkFileOpening();
        void fillWithZeros();

    public:

        explicit ResultWriter(const utils::Path& file_name, uint64_t blocks_count);

        ResultWriter& operator << (const utils::BinaryBlock& block);
    };
}
