#pragma once

#include "utils.h"

#include <string>
#include <memory>

namespace filesys
{
    class FileIterator
    {

    public:
        struct FileDeleter
        {
            using pointer = FILE*;

            void operator ()(pointer f) const
            {
                ::fclose(f);
            }
        };
        
        using UniqueFile      = std::unique_ptr<std::remove_pointer<FILE*>::type, FileDeleter>;
        using BinaryBufferPtr = std::shared_ptr<std::string>;

        FileIterator(const utils::Path& file_name, size_t chunk_size);
        
        BinaryBufferPtr operator * () const;
        FileIterator& operator ++ ();

        [[nodiscard]] bool isMoreDataAvailable() const noexcept;

    private:
        size_t          m_chunk_size;
        size_t          m_file_size;
        utils::Path     m_file_name;
        BinaryBufferPtr m_buffer;
        UniqueFile      m_file_ptr;
        bool            m_more_data;

        void incrementChunk();
    };
}