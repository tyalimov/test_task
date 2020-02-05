#include <gtest/gtest.h>

#include <file_iterator.h>
#include <utils.h>

#include <vector>

using namespace builder;

using utils::Path;
using filesys::FileIterator;

using ChunkList = std::vector<utils::BinaryBufferPtr>;

void GetChunks(const Path& filename, size_t chunk_size, ChunkList& chunks) try
{
    Path file_path = "file_iterator";
    file_path      /= filename;

    ASSERT_TRUE(utils::fs::exists(file_path));

    if (utils::fs::file_size(file_path) == 0)
    {
        auto buf_ptr = std::make_shared<utils::BinaryBuffer>(chunk_size, 0);
        chunks.push_back(buf_ptr);
        return;
    }

    FileIterator iter(file_path, chunk_size);

    while (iter.isMoreDataAvailable())
    {
        ++iter;
        chunks.emplace_back(*iter);
    } 
}
catch (const std::exception& ex)
{
    std::cerr << ex.what() << "\n";
    FAIL();
}

void TestEqual(const ChunkList& chunks, const std::vector<utils::BinaryBuffer>& expected, size_t chunk_size) try
{
    ASSERT_EQ(chunks.size(), expected.size());

    auto chunks_it   = chunks.begin();
    auto expected_it = expected.begin();

    while (chunks_it != chunks.end())
    {
        ASSERT_EQ(**chunks_it, *expected_it);
        ++chunks_it;
        ++expected_it;
    }

    SUCCEED();
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

    std::vector<utils::BinaryBuffer> expected
    {
        { '0', '1','2','3','4','5','6','7','8','9' }
    };

    GetChunks(path, chunk_size, chunks);
    TestEqual(chunks, expected, chunk_size);
}


TEST(file_iterator, Padded_Read_Multiple_Chunks) 
{
    ChunkList chunks;
    Path      path{ "padded_multiple_chunks.txt" };
    size_t    chunk_size{ 10 };

    std::vector<utils::BinaryBuffer> expected
    {
        { '0', '1','2','3','4','5','6','7','8','9' },
        { 'a', 'b','c','d','e','f','g','h','i','j' },
        { '0', '1','2','3','4','5','6','7','8','9' },
        { 'a', 'b','c','d','e','f','g','h','i','j' },
        { '1', '1','1','1','1','1','1','1','1','1' }
    };

    GetChunks(path, chunk_size, chunks);
    TestEqual(chunks, expected, chunk_size);
}

TEST(file_iterator, Unpadded_Read_Single_Chunk) 
{
    ChunkList chunks;
    Path      path{ "unpadded_single_chunk.txt" };
    size_t    chunk_size{ 10 };

    std::vector<utils::BinaryBuffer> expected
    {
        { '0', '1','2','3','4','5','6',0, 0, 0 }
    };

    GetChunks(path, chunk_size, chunks);
    TestEqual(chunks, expected, chunk_size);
}

TEST(file_iterator, Unpadded_Read_Multiple_Chunks) 
{
    ChunkList chunks;
    Path      path{ "unpadded_multiple_chunks.txt" };
    size_t    chunk_size{ 10 };

    std::vector<utils::BinaryBuffer> expected
    {
        { '0', '1','2','3','4','5','6','7','8','9' },
        { 'a', 'b','c','d','e','f','g','h','i','j' },
        { '0', '1','2','3','4','5','6','7','8','9' },
        { 'a', 'b','c','d','e','f','g','h','i','j' },
        { '1', '1','1','1','1','1','1', 0,  0,  0  }
    };

    GetChunks(path, chunk_size, chunks);
    TestEqual(chunks, expected, chunk_size);
}

// We don't need to do padding, because it will be done by crypto library
TEST(file_iterator, Read_Zero_Bytes) 
{
    ChunkList chunks;
    Path      path{ "zero_bytes.txt" };
    size_t    chunk_size{ 10 };

    std::vector<utils::BinaryBuffer> expected
    {
        { 0,0,0,0,0,0,0,0,0,0 }
    };

    GetChunks(path, chunk_size, chunks);
    TestEqual(chunks, expected, chunk_size);
}