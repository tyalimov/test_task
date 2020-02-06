﻿#include "file_iterator.h"

#pragma warning (disable:4996)

#define INITIAL_BLOCK_ID size_t(-1)

namespace builder::filesys
{
    void FileIterator::incrementChunk()
    {
        m_buffer.reset(new utils::BinaryBuffer(m_block_size, static_cast<char>(0)));

        uint64_t file_ptr_pos       = m_file_ptr.tellg();
        uint64_t bytes_left_in_file = m_file_size - file_ptr_pos;
        uint64_t bytes_to_read      = bytes_left_in_file >= m_block_size
                                        ? m_block_size
                                        : bytes_left_in_file;

        if (bytes_left_in_file == bytes_to_read)
        {
            m_more_data = false;
        }

        m_file_ptr.read(reinterpret_cast<char*>(m_buffer->data()), bytes_to_read);

        if (!m_file_ptr)
        {
            throw std::runtime_error("error while reading file");
        }
    }

    FileIterator::FileIterator(const utils::Path &file_name, size_t chunk_size)
        : m_block_size(chunk_size)
        , m_file_size(static_cast<uint64_t>(utils::fs::file_size(file_name)))
        , m_current_block_id(INITIAL_BLOCK_ID)
        , m_more_data(utils::fs::file_size(file_name))
        , m_file_name(file_name)
        , m_file_ptr(file_name.generic_string(), std::ios::binary)
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

            m_current_block_id = m_current_block_id == INITIAL_BLOCK_ID
                ? 0
                : m_current_block_id + 1;
        }
        return (*this);
    }

    bool FileIterator::moreDataAvailable() const noexcept
    {
        return m_more_data;
    }
}
