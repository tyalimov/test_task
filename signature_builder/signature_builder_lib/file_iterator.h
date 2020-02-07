#pragma once

#include "utils.h"

#include <fstream>

namespace builder::filesys
{
    struct BinaryBlock
    {
        size_t                 id;
        utils::BinaryBufferPtr buffer;

        BinaryBlock(size_t idx, const utils::BinaryBufferPtr& buf)
            : id(idx)
            , buffer(buf)
        {}
    };

    class FileIterator
    {
    public:
        FileIterator(const utils::Path& file_name, size_t chunk_size);

        BinaryBlock operator * () const;
        FileIterator& operator ++ ();

        [[nodiscard]] bool moreDataAvailable() const noexcept;

    private:
        size_t                 m_block_size;
        uint64_t               m_file_size;
        size_t                 m_current_block_id;
        bool                   m_more_data;
        utils::Path            m_file_name;
        std::ifstream          m_file_ptr;
        utils::BinaryBufferPtr m_buffer;

        void incrementChunk();
    };
}