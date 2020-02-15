#pragma once

#include <filesystem>
#include <vector>
#include <memory>
#include <string>

// TODO: Перевести в enum
#define SHA512_DIGEST_SIZE 64

namespace builder::utils
{
    namespace fs          = std::filesystem;
        
    using Path            = fs::path;
    using BinaryBuffer    = std::vector<uint8_t>;
    using BinaryBufferPtr = std::shared_ptr<BinaryBuffer>;
    using UniqueBuffer    = std::unique_ptr<BinaryBuffer>;

    std::string ToHex(const std::string& input);
    std::string ReadFile(const Path& file);
    std::vector<uint8_t> ReadBinaryFile(const Path& file_name);
    uint64_t AlignGreater(uint64_t total_size, uint64_t block_size);
    void CreateOutputFile(const Path& input, const Path& output, uint64_t block_size);

    // TODO: мб file_block или как-то так...?
    struct BinaryBlock
    {
        uint64_t        id;
        BinaryBufferPtr buffer;
        BinaryBufferPtr hash;

        BinaryBlock(uint64_t idx, const BinaryBufferPtr& buf)
            : id(idx)
            , buffer(buf)
            , hash(std::make_shared<BinaryBuffer>(SHA512_DIGEST_SIZE, static_cast<uint8_t>(0)))
        {}

        BinaryBlock(uint64_t idx, const BinaryBufferPtr& buf, const BinaryBufferPtr& hash_buf)
            : id(idx)
            , buffer(buf)
            , hash(hash_buf)
        {}

        BinaryBlock()
            : id(0)
            , buffer(std::make_shared<BinaryBuffer>())
            , hash(std::make_shared<BinaryBuffer>(SHA512_DIGEST_SIZE, static_cast<uint8_t>(0)))
        {}
    };
}