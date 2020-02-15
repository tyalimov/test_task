#include <gtest/gtest.h>

#include "file_mapper.h"
#include "utils.h"

// TODO: Добавить нормальные пути к бусту
// TODO: Проверка, что на ласт блок возвращается нормальный сайз

using namespace builder::filesys;
using namespace builder::utils;

TEST(file_mapper, unaligned_file) try
{
    Path file_name{ R"(file_mapper\unaligned.txt)" };
    FileMapper mapper{ file_name, 10 };

    mapper.map();

    ASSERT_EQ(mapper.getTotalBlocks(), 3);

    auto first_block  = mapper.getPtr(0);
    auto second_block = mapper.getPtr(1);
    auto third_block = mapper.getPtr(2);

    ASSERT_EQ(first_block.m_size, 10);
    ASSERT_EQ(second_block.m_size, 10);
    ASSERT_EQ(third_block.m_size, 2);

    ASSERT_EQ("hello from", std::string(reinterpret_cast<char*>(first_block.m_ptr),  first_block.m_size));
    ASSERT_EQ(" file mapp", std::string(reinterpret_cast<char*>(second_block.m_ptr), second_block.m_size));
    ASSERT_EQ("er",        std::string(reinterpret_cast<char*>(third_block.m_ptr),  third_block.m_size));
}
catch (const std::exception& ex)
{
    std::cout << ex.what() << std::endl;
    FAIL();
}


TEST(file_mapper, aligned_file) try
{
    Path file_name{ R"(file_mapper\aligned.txt)" };
    FileMapper mapper{ file_name, 5 };

    mapper.map();

    ASSERT_EQ(mapper.getTotalBlocks(), 4);

    auto first_block  = mapper.getPtr(0);
    auto second_block = mapper.getPtr(1);
    auto third_block  = mapper.getPtr(2);
    auto fourth_block = mapper.getPtr(3);

    ASSERT_EQ(first_block.m_size,  5);
    ASSERT_EQ(second_block.m_size, 5);
    ASSERT_EQ(third_block.m_size,  5);
    ASSERT_EQ(fourth_block.m_size, 5);

    ASSERT_EQ("11111", std::string(reinterpret_cast<char*>(first_block.m_ptr),  first_block.m_size));
    ASSERT_EQ("22222", std::string(reinterpret_cast<char*>(second_block.m_ptr), second_block.m_size));
    ASSERT_EQ("33333", std::string(reinterpret_cast<char*>(third_block.m_ptr),  third_block.m_size));
    ASSERT_EQ("44444", std::string(reinterpret_cast<char*>(fourth_block.m_ptr), fourth_block.m_size));
}
catch (const std::exception& ex)
{
    std::cout << ex.what() << std::endl;
    FAIL();
}

TEST(file_mapper, out_of_range) try
{
    Path file_name{ R"(file_mapper\aligned.txt)" };
    FileMapper mapper{ file_name, 5 };

    mapper.map();

    ASSERT_EQ(mapper.getTotalBlocks(), 4);

    auto fourth_block = mapper.getPtr(4);

    FAIL();

}
catch (const BlockOutOfRange& ex)
{
    std::cout << ex.what() << std::endl;
    SUCCEED();
}
catch (const std::exception& ex)
{
    std::cout << ex.what() << std::endl;
    FAIL();
}

TEST(file_mapper, file_not_mapped) try
{
    Path file_name{ R"(file_mapper\aligned.txt)" };
    FileMapper mapper{ file_name, 5 };


    ASSERT_EQ(mapper.getTotalBlocks(), 4);

    auto fourth_block = mapper.getPtr(4);

    FAIL();

}
catch (const FileNotMapped& ex)
{
    std::cout << ex.what() << std::endl;
    SUCCEED();
}
catch (const std::exception& ex)
{
    std::cout << ex.what() << std::endl;
    FAIL();
}