#include "utils.h"

#include <fstream>
#include <sstream>
#include <iterator>

namespace builder::utils
{
    std::string ToHex(const BinaryBuffer &input)
    {
        static const char *const lut = "0123456789abcdef";
        size_t len = input.size();

        std::string output;
        output.reserve(2 * len);

        for (size_t i = 0; i < len; ++i)
        {
            const uint8_t c = input[i];
            output.push_back(lut[c >> 4]);
            output.push_back(lut[c & 15]);
        }

        return output;
    }

    std::string ReadFile(const Path &file)
    {
        std::ifstream ss(file.generic_string(), std::ios::binary);
        std::stringstream buffer;
        buffer << ss.rdbuf();

        return buffer.str();
    }

    std::vector<uint8_t> ReadBinaryFile(const Path &file_name)
    {
        std::ifstream file(file_name.generic_string(), std::ios::binary);

        file.unsetf(std::ios::skipws);
        file.seekg(0, std::ios::end);
        std::streampos file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> vec;
        vec.reserve(static_cast<size_t>(file_size));

        vec.insert(vec.begin(),
                   std::istream_iterator<uint8_t>(file),
                   std::istream_iterator<uint8_t>());

        return vec;
    }
}
