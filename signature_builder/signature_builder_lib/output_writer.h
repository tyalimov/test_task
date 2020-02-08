#pragma once

#include "utils.h"

#include <fstream>

#define ZERO_BLOCK_SIZE (15 * 1024 * 1024)

namespace builder::filesys
{
    // TODO: file.exceptions ( ofstream::badbit );
    // TODO: Если файл уже существует, слать в жопу (в мейне) 
    // TODO: assert / exception на несовпадение записи блока и размера файла
    class ResultWriter
    {
    private:
        static const char kDataToFill[ZERO_BLOCK_SIZE];
        
        std::ofstream     m_file;
        uint64_t          m_file_size;

        void checkFileOpening() const;
        void fillWithZeros();

    public:
        ResultWriter(const utils::Path& file_name, uint64_t hash_blocks_count);

        // TODO: Группа блоков
        ResultWriter& operator << (const utils::BinaryBlock& block);
    };
}
