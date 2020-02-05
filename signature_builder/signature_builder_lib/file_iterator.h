#pragma once

#include "utils.h"

namespace builder::filesys
{
    class FileIterator
    {
    public:
        FileIterator(const utils::Path& file_name, size_t chunk_size);

        utils::BinaryBufferPtr operator * () const;
        FileIterator& operator ++ ();

        [[nodiscard]] bool isMoreDataAvailable() const noexcept;

    private:
        size_t                 m_chunk_size;
        size_t                 m_file_size;
        bool                   m_more_data;
        utils::Path            m_file_name;
        utils::FilePtr         m_file_ptr;
        utils::BinaryBufferPtr m_buffer;

        void incrementChunk();
    };
}