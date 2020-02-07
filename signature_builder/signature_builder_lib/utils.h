#pragma once

#include <filesystem>
#include <vector>
#include <memory>

#define SHA512_DIGEST_SIZE 64

namespace builder::utils
{
    namespace fs = std::filesystem;

    struct FileDeleter
    {
        using pointer = FILE*;

        void operator ()(pointer f) const
        {
            ::fclose(f);
        }
    };
        
    using Path            = fs::path;
    using BinaryBuffer    = std::vector<uint8_t>;
    using BinaryBufferPtr = std::shared_ptr<BinaryBuffer>;
    using FilePtr         = std::unique_ptr<std::remove_pointer<FILE*>::type, FileDeleter>;
}