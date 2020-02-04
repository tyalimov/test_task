#include <gtest/gtest.h>

#include <file_iterator.h>
#include <utils.h>

#include <filesystem>
#include <vector>

using utils::Path;
using filesys::FileIterator;

using ChunkList = std::vector<FileIterator::BinaryBufferPtr>;

void GetChunks(const Path& filename, size_t chunk_size, ChunkList& chunks) try
{
    Path file_path = "file_iterator";
    file_path      /= filename;

    EXPECT_TRUE(utils::fs::exists(file_path));

    FileIterator iter(filename, chunk_size);

    while(iter.isMoreDataAvailable())
    {
        chunks.emplace_back(*iter);
    }
}
catch (const std::exception& ex)
{
    std::cerr << ex.what() << "\n";
    FAIL();
}

void TestEqual(const ChunkList& chunks, std::vector<std::string> expected, size_t chunk_size) try
{
    EXPECT_EQ(chunks.size(), expected.size());

    auto chunks_it   = chunks.begin();
    auto expected_it = expected.begin();

    while (chunks_it != chunks.end())
    {
        EXPECT_EQ(**chunks_it, *expected_it);
    }
}
catch (const std::exception& ex)
{
    std::cerr << ex.what() << "\n";
    FAIL();
}


TEST(file_iterator, Padded_Read_Single_Chunk)
{
    ChunkList chunks;
    Path      path{ "padded_single_chunk.txt" };
    size_t    chunk_size{ 10 };

    std::vector<std::string> expected
    {
        "0123456789"
    };

    GetChunks(path, chunk_size, chunks);
    TestEqual(chunks, expected, chunk_size);
}


TEST(file_iterator, Padded_Read_Multiple_Chunks) 
{
    ChunkList chunks;
    Path      path{ "padded_multiple_chunks.txt" };
    size_t    chunk_size{ 10 };

    std::vector<std::string> expected
    {
        "0123456789",
        "abcdefghij",
        "0123456789",
        "abcdefghij",
        "1111111111"
    };

    GetChunks(path, chunk_size, chunks);
    TestEqual(chunks, expected, chunk_size);
}

TEST(file_iterator, Unpadded_Read_Single_Chunk) 
{
    ChunkList chunks;
    Path      path{ "unpadded_single_chunk.txt" };
    size_t    chunk_size{ 10 };

    std::vector<std::string> expected
    {
        "0123456\0\0\0",
    };

    GetChunks(path, chunk_size, chunks);
    TestEqual(chunks, expected, chunk_size);
}

TEST(file_iterator, Unpadded_Read_Multiple_Chunks) 
{
    ChunkList chunks;
    Path      path{ "unpadded_multiple_chunks.txt" };
    size_t    chunk_size{ 10 };

    std::vector<std::string> expected
    {
        "0123456789",
        "abcdefghij",
        "0123456789",
        "abcdefghij",
        "1111111\0\0\0"
    };

    GetChunks(path, chunk_size, chunks);
    TestEqual(chunks, expected, chunk_size);
}

TEST(file_iterator, Read_Zero_Bytes) 
{
    ChunkList chunks;
    Path      path{ "zero_bytes.txt" };
    size_t    chunk_size{ 10 };

    std::vector<std::string> expected
    {
        "\0\0\0\0\0\0\0\0\0\0",
    };

    GetChunks(path, chunk_size, chunks);
    TestEqual(chunks, expected, chunk_size);
}