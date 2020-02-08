#include "input_reader.h"

#define INITIAL_BLOCK_ID uint64_t(-1)

namespace builder::filesys
{
    void InputReader::readNextBlock()
    {
        if (m_zero_sized_file)
        {
            m_buffer.reset(new utils::BinaryBuffer(static_cast<size_t>(m_block_size), static_cast<char>(0)));
            m_zero_sized_file = false;
            m_more_data       = false;
            return;
        }

        if (!m_more_data)
        {
            throw std::runtime_error("there is no more data file");
        }

        m_buffer.reset(new utils::BinaryBuffer(static_cast<size_t>(m_block_size), static_cast<char>(0)));

        uint64_t file_ptr_pos       = m_file.tellg();
        uint64_t bytes_left_in_file = m_file_size - file_ptr_pos;
        uint64_t bytes_to_read      = bytes_left_in_file >= m_block_size
                                        ? m_block_size
                                        : bytes_left_in_file;

        if (bytes_left_in_file == bytes_to_read)
        {
            m_more_data = false;
        }

        m_file.read(reinterpret_cast<char*>(m_buffer->data()), bytes_to_read);

        if (!m_file)
        {
            throw std::runtime_error("error while reading file");
        }
    }

    void InputReader::checkFileOpening() const
    {
        if (!m_file.is_open())
        {
            // TODO: Ну тут точно надо юзнуть свои эксепшны
            throw std::runtime_error("can't open file");
        }
    }

    void InputReader::incrementBlockId() noexcept
    {
        m_current_block_id = m_current_block_id == INITIAL_BLOCK_ID
            ? 0
            : m_current_block_id + 1;
    }

    InputReader::InputReader(const utils::Path& file_name, uint64_t block_size)
        : m_block_size(block_size)
        , m_file_size(static_cast<uint64_t>(utils::fs::file_size(file_name)))
        , m_current_block_id(INITIAL_BLOCK_ID)
        , m_more_data(true)
        , m_zero_sized_file(!utils::fs::file_size(file_name))
        , m_file(file_name.generic_string(), std::ios::binary)
        , m_buffer(std::make_shared<utils::BinaryBuffer>(static_cast<size_t>(block_size), static_cast<uint8_t>(0)))
    {
        checkFileOpening();
    }

    InputReader& InputReader::operator >> (utils::BinaryBlock& block)
    {
        readNextBlock();
        incrementBlockId();

        block = utils::BinaryBlock(m_current_block_id, m_buffer);

        return (*this);
    }

    InputReader::operator bool() const noexcept
    {
        return m_more_data;
    }
}
