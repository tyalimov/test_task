#include "file_iterator.h"

#include <algorithm>

#pragma warning (disable:4996)

// TODO: fopen
// TODO: own exceptions
// TODO: Так-ли нужно юзать CRT, может все-таки STL?
// TODO: fopen еще и варнинги кидает!

namespace builder::filesys
{
    void FileIterator::incrementChunk()
    {
        m_buffer.reset(new std::string(m_chunk_size, static_cast<char>(0)));

        auto file_ptr_pos = ftell(m_file_ptr.get());

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
        , m_file_name(file_name)
        , m_buffer(std::make_shared<std::string>(chunk_size, static_cast<char>(0)))
        , m_file_ptr(fopen(file_name.generic_string().c_str(), "rb"))
        , m_more_data(utils::fs::file_size(file_name))
    {
        if (!m_file_ptr.get())
        {
            throw std::runtime_error("can't open file for reading");
        }

        //incrementChunk();
    }

    FileIterator::BinaryBufferPtr FileIterator::operator*() const
    {
        return m_buffer;
    }

    FileIterator& FileIterator::operator++()
    {
        if (m_more_data)
        {
            incrementChunk();
        }
        return (*this);
    }

    bool FileIterator::isMoreDataAvailable() const noexcept
    {
        return m_more_data;
    }
}
