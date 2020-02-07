#include "reult_writer.h"

namespace builder::filesys
{
    const char ResultWriter::kDataToFill[] = { static_cast<char>(0) };

    void ResultWriter::checkFileOpening() const
    {
        if (m_file.is_open())
        {
            // TODO: Ну тут точно надо юзнуть свои эксепшны
            throw std::runtime_error("can't open file");
        }
    }

    void ResultWriter::fillWithZeros()
    {
        uint64_t zero_blocks_count = ZERO_BLOCK_SIZE % m_file_size 
            ? ZERO_BLOCK_SIZE / m_file_size + 1 
            : ZERO_BLOCK_SIZE / m_file_size;

        uint64_t last_block_size = ZERO_BLOCK_SIZE % m_file_size;

        for (uint64_t i = 0; i < zero_blocks_count - 1; i++)
        {
            m_file.write(kDataToFill, ZERO_BLOCK_SIZE);
        }

        m_file.write(kDataToFill, last_block_size);
    }

    ResultWriter::ResultWriter(const utils::Path &file_name, uint64_t blocks_count)
        : m_file(file_name.generic_string(), std::ios::binary)
        , m_file_size(blocks_count * SHA512_DIGEST_SIZE)
    {
        checkFileOpening();
        fillWithZeros();
    }

    ResultWriter& ResultWriter::operator << (const utils::BinaryBlock& block)
    {
        uint64_t offset = block.id * SHA512_DIGEST_SIZE; //  TODO: -1?

        m_file.seekp(offset);
        m_file.write(reinterpret_cast<char*>(block.hash.get()), SHA512_DIGEST_SIZE);

        return (*this);
    }
}