#pragma once

#include "utils.h"

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

        [[nodiscard]] bool isMoreDataAvailable() const noexcept;

    private:
        size_t                 m_chunk_size;
        size_t                 m_file_size;
        size_t                 m_current_block_id;
        bool                   m_more_data;
        utils::Path            m_file_name;
        utils::FilePtr         m_file_ptr;
        utils::BinaryBufferPtr m_buffer;

        static utils::FilePtr openFile(const utils::Path& file_name);
        void incrementChunk();
    };
}