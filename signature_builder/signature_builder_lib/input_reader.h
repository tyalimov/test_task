#pragma once

#include "utils.h"

#include <fstream>

namespace builder::filesys
{
    // TODO: file.exceptions ( ifstream::badbit );
    // TODO: Наследование от общего класса с writer?
    class InputReader
    {
    public:
        InputReader(const utils::Path& file_name, uint64_t block_size);
        InputReader& operator >> (utils::BinaryBlock& block);

        explicit operator bool() const noexcept;

    private:
        uint64_t               m_block_size;
        uint64_t               m_file_size;
        uint64_t               m_current_block_id;
        bool                   m_more_data;
        bool                   m_zero_sized_file;
        std::ifstream          m_file;
        utils::BinaryBufferPtr m_buffer;

        void readNextBlock();
        void incrementBlockId() noexcept;
        void checkFileOpening() const;
    };
}