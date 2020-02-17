#include <misc/utils.h>
#include <crypto/hash_maker.h>

#include <fstream>

#include <gtest/gtest.h>

using namespace builder;

using utils::Path;

void TestHashMaker(const std::string& expected, const Path& file) try
{
    Path filename = "hash_maker";
    filename /= file;

    EXPECT_TRUE(utils::fs::exists(filename));

    auto file_data     = utils::ReadFile(filename.generic_string());
    auto hash          = crypto::HashMaker((uint8_t*)file_data.c_str(),file_data.size()).getHash();

    EXPECT_EQ
    (
        utils::ToHex(std::string((char*)hash.data(), utils::constants::kDigestSize)), 
        expected
    );
}
catch (const std::exception& ex)
{
    std::cerr << ex.what() << std::endl;
    FAIL();
}

TEST(hash_maker, Zero_bytes)
{
    std::string expected{ "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e" };

    TestHashMaker(expected, "zero_bytes.txt");
}

TEST(hash_maker, Half_block)
{
    std::string expected{ "d7f7b4b1862fe2d4da0f0ee0cf69aa00291a11cc6bfddcbb3dd7f30a608864fe6ecaf9cb59f334138d6d6981ceaa3742890a2b30d077634483396429edf53ff0" };

    TestHashMaker(expected, "half_block.txt");
}

TEST(hash_maker, Full_block)
{
    std::string expected{ "bacd075be4216b75875ace1f2bcd96f664c50894a804690669fd45df7874fa24e0d017effb5917dbb597d4c2c873b2ff195e7d6d27236e32fceb9fcb6ce33309" };

    TestHashMaker(expected, "full_block.txt");
}

TEST(hash_maker, More_than_full_block)
{
    std::string expected{ "66446782019e74034038f4b052926fc239bb51fa2e77e5e31b39907822ce9e1d0d43e6fc9e4095fd9ee3c6d394c816aec869d2c4de4643e4ef8aacc183fecb70" };
    
    TestHashMaker(expected, "more_than_full_block.txt");
}