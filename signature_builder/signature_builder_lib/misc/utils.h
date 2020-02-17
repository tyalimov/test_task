#pragma once

#include <filesystem>
#include <vector>
#include <string>

namespace builder::utils
{
    namespace constants
    {
        enum Constants
            : uint64_t
        {
            k1Gb               = 1024 * 1024 * 1024,
            kDigestSize        = 64,
            kDefaultBlockSize  = 1024 * 1024
        };
    }

    namespace fs = std::filesystem;
        
    using Path = fs::path;

    std::string ToHex(const std::string& input);
    std::string ReadFile(const Path& file);
    std::vector<uint8_t> ReadBinaryFile(const Path& file_name);
    uint64_t AlignGreater(uint64_t total_size, uint64_t block_size);
    void CreateOutputFile(const Path& input, const Path& output, uint64_t block_size);
}