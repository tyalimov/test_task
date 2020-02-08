#include <gtest/gtest.h>

#include <input_reader.h>
#include <utils.h>

#include <vector>

using namespace builder;

using utils::Path;
using filesys::InputReader;

using BlockList = std::vector<utils::BinaryBlock>;

void GetBlocks(const Path& filename, size_t block_size, BlockList& blocks) try
{
    Path file_path = "input_reader";
    file_path      /= filename;

    ASSERT_TRUE(utils::fs::exists(file_path));

    InputReader input_reader(file_path, block_size);

    while (input_reader) //-V1044
    {
        utils::BinaryBlock buf;
        input_reader >> buf;
        blocks.push_back(buf);
    } 
}
catch (const std::exception& ex)
{
    std::cerr << ex.what() << "\n";
    FAIL();
}

void TestEqual(const BlockList& blocks, const std::vector<utils::BinaryBuffer>& expected, size_t block_size) try
{
    ASSERT_EQ(blocks.size(), expected.size());

    auto blocks_it   = blocks.begin();
    auto expected_it = expected.begin();

    size_t block_id{ 0 };

    while (blocks_it != blocks.end())
    {
        ASSERT_EQ(*(blocks_it->buffer), *expected_it);
        ASSERT_EQ(blocks_it->id, block_id++);
        ++blocks_it;
        ++expected_it;
    }

    SUCCEED();
}
catch (const std::exception& ex)
{
    std::cerr << ex.what() << "\n";
    FAIL();
}


TEST(input_reader, Padded_Read_Single_Block)
{
    BlockList blocks;
    Path      path{ "padded_single_block.txt" };
    size_t    block_size{ 10 };

    std::vector<utils::BinaryBuffer> expected
    {
        { '0', '1','2','3','4','5','6','7','8','9' }
    };

    GetBlocks(path, block_size, blocks);
    TestEqual(blocks, expected, block_size);
}


TEST(input_reader, Padded_Read_Multiple_blocks) 
{
    BlockList blocks;
    Path      path{ "padded_multiple_blocks.txt" };
    size_t    block_size{ 10 };

    std::vector<utils::BinaryBuffer> expected
    {
        { '0', '1','2','3','4','5','6','7','8','9' },
        { 'a', 'b','c','d','e','f','g','h','i','j' },
        { '0', '1','2','3','4','5','6','7','8','9' },
        { 'a', 'b','c','d','e','f','g','h','i','j' },
        { '1', '1','1','1','1','1','1','1','1','1' }
    };

    GetBlocks(path, block_size, blocks);
    TestEqual(blocks, expected, block_size);
}

TEST(input_reader, Unpadded_Read_Single_Block) 
{
    BlockList blocks;
    Path      path{ "unpadded_single_block.txt" };
    size_t    block_size{ 10 };

    std::vector<utils::BinaryBuffer> expected
    {
        { '0', '1','2','3','4','5','6',0, 0, 0 }
    };

    GetBlocks(path, block_size, blocks);
    TestEqual(blocks, expected, block_size);
}

TEST(input_reader, Unpadded_Read_Multiple_blocks) 
{
    BlockList blocks;
    Path      path{ "unpadded_multiple_blocks.txt" };
    size_t    block_size{ 10 };

    std::vector<utils::BinaryBuffer> expected
    {
        { '0', '1','2','3','4','5','6','7','8','9' },
        { 'a', 'b','c','d','e','f','g','h','i','j' },
        { '0', '1','2','3','4','5','6','7','8','9' },
        { 'a', 'b','c','d','e','f','g','h','i','j' },
        { '1', '1','1','1','1','1','1', 0,  0,  0  }
    };

    GetBlocks(path, block_size, blocks);
    TestEqual(blocks, expected, block_size);
}

// We don't need to do padding, because it will be done by crypto library
TEST(input_reader, Read_Zero_Bytes) 
{
    BlockList blocks;
    Path      path{ "zero_bytes.txt" };
    size_t    block_size{ 10 };

    std::vector<utils::BinaryBuffer> expected
    {
        { 0,0,0,0,0,0,0,0,0,0 }
    };

    GetBlocks(path, block_size, blocks);
    TestEqual(blocks, expected, block_size);
}