#include "file_iterator.h"

#include <algorithm>

#pragma warning (disable:4996)

#define INITIAL_BLOCK_ID size_t(-1)

// TODO: fopen
// TODO: own exceptions
// TODO: Так-ли нужно юзать CRT, может все-таки STL?
// TODO: fopen еще и варнинги кидает!

namespace builder::filesys
{
    utils::FilePtr FileIterator::openFile(const utils::Path &file_name)
    {
        auto ptr = fopen(file_name.generic_string().c_str(), "rb");

        if (!ptr)
        {
            throw std::runtime_error("can't open file for reading");
        }

        return utils::FilePtr(ptr);
    }

    void FileIterator::incrementChunk()
    {
        m_buffer.reset(new utils::BinaryBuffer(m_chunk_size, static_cast<char>(0)));

        auto file_ptr_pos = _ftelli64(m_file_ptr.get());

        if (file_ptr_pos == -1L)
        {
            throw std::runtime_error("error while getting file size");
        }

        auto bytes_left_in_file = m_file_size - file_ptr_pos;
        auto bytes_to_read = bytes_left_in_file >= m_chunk_size
            ? m_chunk_size
            : bytes_left_in_file;

        if (bytes_left_in_file == bytes_to_read)
        {
            m_more_data = false;
        }

        auto bytes_read = fread
        (
            m_buffer->data(),
            sizeof(char),
            bytes_to_read,
            m_file_ptr.get()
        );

        if (bytes_read != bytes_to_read)
        {
            throw std::runtime_error("error while reading file");
        }
    }

    FileIterator::FileIterator(const utils::Path &file_name, size_t chunk_size)
        : m_chunk_size(chunk_size)
        , m_file_size(static_cast<size_t>(utils::fs::file_size(file_name)))
        , m_current_block_id(INITIAL_BLOCK_ID)
        , m_more_data(utils::fs::file_size(file_name))
        , m_file_name(file_name)
        , m_file_ptr(openFile(file_name))
        , m_buffer(std::make_shared<utils::BinaryBuffer>(chunk_size, static_cast<uint8_t>(0)))
    {
    }

    BinaryBlock FileIterator::operator*() const
    {
        return BinaryBlock{ m_current_block_id, m_buffer };
    }

    FileIterator& FileIterator::operator++()
    {
        if (m_more_data)
        {
            incrementChunk();

            if (m_current_block_id == INITIAL_BLOCK_ID)
            {
                m_current_block_id = 0;
            }
            else
            {
                m_current_block_id++;
            }
        }
        return (*this);
    }

    bool FileIterator::moreDataAvailable() const noexcept
    {
        return m_more_data;
    }
}
