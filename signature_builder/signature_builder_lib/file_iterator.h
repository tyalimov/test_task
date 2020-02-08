#pragma once

#include "utils.h"

#include <fstream>

namespace builder::filesys
{
    // TODO: file.exceptions ( ifstream::badbit );
    class FileIterator
    {
    public:
        FileIterator(const utils::Path& file_name, size_t chunk_size);

        utils::BinaryBlock operator *  () const;
        FileIterator&      operator ++ ();

        // TODO: operator bool
        [[nodiscard]] bool moreDataAvailable() const noexcept;

    private:
        size_t                 m_block_size;
        uint64_t               m_file_size;
        size_t                 m_current_block_id;
        bool                   m_more_data;
        utils::Path            m_file_name;
        std::ifstream          m_file;
        utils::BinaryBufferPtr m_buffer;

        void incrementChunk();
    };
}