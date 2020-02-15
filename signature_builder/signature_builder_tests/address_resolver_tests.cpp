#include <gtest/gtest.h>

#include "address_resolver.h"

using namespace builder::filesys;

void TestResolver
(
    uint64_t         file_size, 
    uint64_t         page_size, 
    uint64_t         block_size, 
    uint64_t         start, 
    uint64_t         count,
    MappingAddresses expected
)
{
    auto actual = AddressResolver{ page_size, block_size, file_size }.resolve(start, count);

    ASSERT_EQ(expected.m_mapping_start, actual.m_mapping_start);
    ASSERT_EQ(expected.m_blocks_start,  actual.m_blocks_start);
    ASSERT_EQ(expected.m_mapping_size,  actual.m_mapping_size);
    ASSERT_EQ(expected.m_blocks_size,   actual.m_blocks_size);
}

void TestResolverNormal
(
    uint64_t         file_size, 
    uint64_t         page_size, 
    uint64_t         block_size, 
    uint64_t         start, 
    uint64_t         count,
    MappingAddresses expected
)
try
{
    TestResolver(file_size, page_size, block_size, start, count, expected);
}
catch (const std::exception& ex)
{
    std::cout << ex.what() << std::endl;
    FAIL();
}

void TestResolverAbort
(
    uint64_t         file_size, 
    uint64_t         page_size, 
    uint64_t         block_size, 
    uint64_t         start, 
    uint64_t         count,
    MappingAddresses expected
)
try
{
    TestResolver(file_size, page_size, block_size, start, count, expected);
}
catch (const std::out_of_range& ex)
{
    std::cout << ex.what() << std::endl;
    SUCCEED();
}
catch (const std::exception& ex)
{
    std::cout << ex.what() << std::endl;
    FAIL();
}


 
// PAGES           : --++--++--++ 
// BLOCKS          : --++--++--++
// SELECTED BLOCKS : ####
TEST(address_resolver, one)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 2;
    uint64_t block_size      = 2;
    uint64_t start           = 0;
    uint64_t count           = 2;
    expected.m_mapping_start = 0;
    expected.m_mapping_size  = 4;
    expected.m_blocks_start  = 0;
    expected.m_blocks_size   = 4;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : --++--++--++ 
// BLOCKS          : --++--++--++
// SELECTED BLOCKS :         ####
TEST(address_resolver, two)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 2;
    uint64_t block_size      = 2;
    uint64_t start           = 4;
    uint64_t count           = 2;
    expected.m_mapping_start = 8;
    expected.m_mapping_size  = 4;
    expected.m_blocks_start  = 8;
    expected.m_blocks_size   = 4;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : --++--++--++ 
// BLOCKS          : --++--++--++
// SELECTED BLOCKS :     ####
TEST(address_resolver, three)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 2;
    uint64_t block_size      = 2;
    uint64_t start           = 2;
    uint64_t count           = 2;
    expected.m_mapping_start = 4;
    expected.m_mapping_size  = 4;
    expected.m_blocks_start  = 4;
    expected.m_blocks_size   = 4;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : --++--++--++ 
// BLOCKS          : ---+++---+++
// SELECTED BLOCKS : ######
TEST(address_resolver, four)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 2;
    uint64_t block_size      = 3;
    uint64_t start           = 0;
    uint64_t count           = 2;
    expected.m_mapping_start = 0;
    expected.m_mapping_size  = 6;
    expected.m_blocks_start  = 0;
    expected.m_blocks_size   = 6;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : --++--++--++ 
// BLOCKS          : ---+++---+++
// SELECTED BLOCKS :       ###
TEST(address_resolver, five)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 2;
    uint64_t block_size      = 3;
    uint64_t start           = 2;
    uint64_t count           = 1;
    expected.m_mapping_start = 6;
    expected.m_mapping_size  = 4;
    expected.m_blocks_start  = 6;
    expected.m_blocks_size   = 3;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}


// PAGES           : --++--++--++ 
// BLOCKS          : ---+++---+++
// SELECTED BLOCKS :          ###
TEST(address_resolver, six)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 2;
    uint64_t block_size      = 3;
    uint64_t start           = 3;
    uint64_t count           = 1;
    expected.m_mapping_start = 8;
    expected.m_mapping_size  = 4;
    expected.m_blocks_start  = 9;
    expected.m_blocks_size   = 3;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : --++--++--++ 
// BLOCKS          : ---+++---+++
// SELECTED BLOCKS : ###
TEST(address_resolver, seven)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 2;
    uint64_t block_size      = 3;
    uint64_t start           = 0;
    uint64_t count           = 1;
    expected.m_mapping_start = 0;
    expected.m_mapping_size  = 4;
    expected.m_blocks_start  = 0;
    expected.m_blocks_size   = 3;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : --++--++--++ 
// BLOCKS          : --++--++--++
// SELECTED BLOCKS :             ##
TEST(address_resolver, eight)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 2;
    uint64_t block_size      = 2;
    uint64_t start           = 6;
    uint64_t count           = 1;
    expected.m_mapping_start = 0; // Does not matter in this test
    expected.m_mapping_size  = 0; // Does not matter in this test
    expected.m_blocks_start  = 0; // Does not matter in this test
    expected.m_blocks_size   = 0; // Does not matter in this test

    TestResolverAbort(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : --++--++--++ 
// BLOCKS          : --++--++--++
// SELECTED BLOCKS :           ####
TEST(address_resolver, nine)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 2;
    uint64_t block_size      = 2;
    uint64_t start           = 5;
    uint64_t count           = 2;
    expected.m_mapping_start = 0; // Does not matter in this test
    expected.m_mapping_size  = 0; // Does not matter in this test
    expected.m_blocks_start  = 0; // Does not matter in this test
    expected.m_blocks_size   = 0; // Does not matter in this test

    TestResolverAbort(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : ---+++---+++
// BLOCKS          : --++--++--++ 
// SELECTED BLOCKS : ##
TEST(address_resolver, ten)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 3;
    uint64_t block_size      = 2;
    uint64_t start           = 0;
    uint64_t count           = 1;
    expected.m_mapping_start = 0;
    expected.m_mapping_size  = 3;
    expected.m_blocks_start  = 0;
    expected.m_blocks_size   = 2;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : ---+++---+++
// BLOCKS          : --++--++--++ 
// SELECTED BLOCKS :     ##
TEST(address_resolver, eleven)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 3;
    uint64_t block_size      = 2;
    uint64_t start           = 2;
    uint64_t count           = 1;
    expected.m_mapping_start = 3;
    expected.m_mapping_size  = 3;
    expected.m_blocks_start  = 4;
    expected.m_blocks_size   = 2;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : ---+++---+++
// BLOCKS          : --++--++--++ 
// SELECTED BLOCKS :         ##
TEST(address_resolver, twelve)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 3;
    uint64_t block_size      = 2;
    uint64_t start           = 4;
    uint64_t count           = 1;
    expected.m_mapping_start = 6;
    expected.m_mapping_size  = 6;
    expected.m_blocks_start  = 8;
    expected.m_blocks_size   = 2;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : ---+++---+++
// BLOCKS          : --++--++--++ 
// SELECTED BLOCKS :           ##
TEST(address_resolver, thirteen)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 12;
    uint64_t page_size       = 3;
    uint64_t block_size      = 2;
    uint64_t start           = 5;
    uint64_t count           = 1;
    expected.m_mapping_start = 9;
    expected.m_mapping_size  = 3;
    expected.m_blocks_start  = 10;
    expected.m_blocks_size   = 2;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}

// PAGES           : --++--++--++-
// BLOCKS          : --++--++--++-- 
// SELECTED BLOCKS :             ##
TEST(address_resolver, Unaligned_File)
{
    MappingAddresses expected;
    
    uint64_t file_size       = 13;
    uint64_t page_size       = 2;
    uint64_t block_size      = 2;
    uint64_t start           = 6;
    uint64_t count           = 1;
    expected.m_mapping_start = 12;
    expected.m_mapping_size  = 2;
    expected.m_blocks_start  = 12;
    expected.m_blocks_size   = 1;

    TestResolverNormal(file_size, page_size, block_size, start, count, expected);
}