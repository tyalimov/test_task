#include <gtest/gtest.h>

#include <utils.h>
#include <result_writer.h>

#include <vector>
#include <fstream>

using namespace builder;

using builder::filesys::ResultWriter;
using utils::Path;

// block_size = 1
// block_size = (15 * 1024 * 1024 / 64)
// block_size = (15 * 1024 * 1024 / 64) + 1

// Тесты BinaryBlock


#define OUT_FILE "result_writer\\out.bin"

void InitTest()
{
    std::remove(OUT_FILE);
}

void TestAdding
(
    const Path& file_name,
    uint64_t blocks_count,
    const std::vector<uint8_t>& expected_file_state,
    const utils::BinaryBlock& block
)
{
    ResultWriter writer(file_name, blocks_count);

    ASSERT_EQ(utils::fs::file_size(file_name), expected_file_state.size());

    writer << block;

    auto file_state = utils::ReadBinaryFile(file_name);

    ASSERT_EQ(file_state, expected_file_state);
}

TEST(result_writer, Creation_1_Block)
{
    InitTest();

    auto file_name     = OUT_FILE;
    auto blocks_count  = 1;
    auto expected_size = SHA512_DIGEST_SIZE * blocks_count;

    ResultWriter writer(file_name, blocks_count);

    ASSERT_EQ(utils::fs::file_size(file_name), expected_size);
}

TEST(result_writer, Creation_Padded_Block)
{
    InitTest();
    
    auto file_name     = OUT_FILE;
    auto blocks_count  = ZERO_BLOCK_SIZE / SHA512_DIGEST_SIZE;
    auto expected_size = SHA512_DIGEST_SIZE * blocks_count;

    ResultWriter writer(file_name, blocks_count);

    ASSERT_EQ(utils::fs::file_size(file_name), expected_size);
}

TEST(result_writer, Creation_Padded_Block_Plus_One)
{
    InitTest();
        
    auto file_name     = OUT_FILE;
    auto blocks_count  = (ZERO_BLOCK_SIZE / SHA512_DIGEST_SIZE) + 1;
    auto expected_size = SHA512_DIGEST_SIZE * blocks_count;

    ResultWriter writer(file_name, blocks_count);

    ASSERT_EQ(utils::fs::file_size(file_name), expected_size);
}

TEST(result_writer,Adding_First_Sha_Block)
{
    InitTest();
        
    auto file_name     = OUT_FILE;
    auto blocks_count  = 3;
    auto block_id      = 0;

    std::vector<uint8_t> expected_file_state
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
  

    utils::BinaryBufferPtr buf  = std::make_shared<utils::BinaryBuffer>();
    utils::BinaryBufferPtr hash = std::make_shared<utils::BinaryBuffer>(SHA512_DIGEST_SIZE, static_cast<uint8_t>(1));

    utils::BinaryBlock block(block_id, buf, hash);

    TestAdding(file_name, blocks_count, expected_file_state, block);
}

TEST(result_writer,Adding_Middle_Sha_Block)
{
    InitTest();
        
    auto file_name     = OUT_FILE;
    auto blocks_count  = 3;
    auto block_id      = 1;

    std::vector<uint8_t> expected_file_state
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
  

    utils::BinaryBufferPtr buf  = std::make_shared<utils::BinaryBuffer>();
    utils::BinaryBufferPtr hash = std::make_shared<utils::BinaryBuffer>(SHA512_DIGEST_SIZE, static_cast<uint8_t>(1));

    utils::BinaryBlock block(block_id, buf, hash);

    TestAdding(file_name, blocks_count, expected_file_state, block);

}

TEST(result_writer,Adding_Last_Sha_Block)
{
    InitTest();
        
    auto file_name     = OUT_FILE;
    auto blocks_count  = 3;
    auto block_id      = 2;

    std::vector<uint8_t> expected_file_state
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
  

    utils::BinaryBufferPtr buf  = std::make_shared<utils::BinaryBuffer>();
    utils::BinaryBufferPtr hash = std::make_shared<utils::BinaryBuffer>(SHA512_DIGEST_SIZE, static_cast<uint8_t>(1));

    utils::BinaryBlock block(block_id, buf, hash);

    TestAdding(file_name, blocks_count, expected_file_state, block);

}