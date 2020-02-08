#include "result_writer.h"

namespace builder::filesys
{
    const char ResultWriter::kDataToFill[] = { static_cast<char>(0) };

    void ResultWriter::checkFileOpening() const
    {
        if (!m_file.is_open())
        {
            // TODO: Ну тут точно надо юзнуть свои эксепшны
            throw std::runtime_error("can't open file");
        }
    }

    void ResultWriter::fillWithZeros()
    {
        if (m_file_size <= ZERO_BLOCK_SIZE)
        {
            m_file.write(kDataToFill, m_file_size);
        }
        else
        {
            uint64_t zero_blocks_count = m_file_size % ZERO_BLOCK_SIZE 
                ? (m_file_size / ZERO_BLOCK_SIZE) + 1 
                : (m_file_size / ZERO_BLOCK_SIZE);

            uint64_t last_block_size = m_file_size % ZERO_BLOCK_SIZE;

            for (uint64_t i = 0; i < zero_blocks_count - 1; i++)
            {
                m_file.write(kDataToFill, ZERO_BLOCK_SIZE);
            }

            m_file.write(kDataToFill, last_block_size);
        }

        m_file.flush();
    }

    ResultWriter::ResultWriter(const utils::Path &file_name, uint64_t hash_blocks_count)
        : m_file(file_name.generic_string(), std::fstream::out | std::fstream::binary | std::fstream::trunc)
        , m_file_size(hash_blocks_count * SHA512_DIGEST_SIZE)
    {
        checkFileOpening();
        fillWithZeros();
    }

    ResultWriter& ResultWriter::operator << (const utils::BinaryBlock& block)
    {
        uint64_t offset = block.id * SHA512_DIGEST_SIZE; //  TODO: -1?

        m_file.seekp(offset);
        m_file.write(reinterpret_cast<char*>(block.hash->data()), SHA512_DIGEST_SIZE);
        m_file.flush();

        return (*this);
    }
}