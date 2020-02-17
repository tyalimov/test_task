#include "output_file_formatter.h"

namespace builder::filesys
{
    const char OutputFileFormatter::kDataToFill[] = { static_cast<char>(0) };

    void OutputFileFormatter::checkFileOpening() const
    {
        if (!m_file.is_open())
        {
            throw FileNotOpened(m_file_name.generic_string());
        }
    }

    void OutputFileFormatter::fillWithZeros()
    {
        if (m_file_size <= constants::kFillingBlockSize)
        {
            m_file.write(kDataToFill, m_file_size);
        }
        else
        {
            uint64_t zero_blocks_count = m_file_size % constants::kFillingBlockSize 
                ? (m_file_size / constants::kFillingBlockSize) + 1 
                : (m_file_size / constants::kFillingBlockSize);

            uint64_t last_block_size = m_file_size % constants::kFillingBlockSize;

            for (uint64_t i = 0; i < zero_blocks_count - 1; i++)
            {
                m_file.write(kDataToFill, constants::kFillingBlockSize);
            }

            m_file.write(kDataToFill, last_block_size);
        }

        m_file.flush();
    }

    OutputFileFormatter::OutputFileFormatter(const utils::Path& file_name, uint64_t hash_blocks_count)
        : m_file(file_name.generic_string(), std::fstream::out | std::fstream::binary | std::fstream::trunc)
        , m_file_size(hash_blocks_count * utils::constants::kDigestSize)
        , m_file_name(file_name)

    {
        checkFileOpening();
        fillWithZeros();
    }

    FileNotOpened::FileNotOpened(const std::string &filename)
        : m_message("Error while opening file - [")
    {
        m_message += filename;
        m_message += "]";
    }

    const char *FileNotOpened::what() const noexcept
    {
        return m_message.c_str();
    }
}
