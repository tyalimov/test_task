#pragma once

#include "utils.h"

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>

#include <vector>

namespace builder::filesys
{
    namespace bip = boost::interprocess;

    struct MappedBlock
    {
        uint8_t  *m_ptr;
        uint64_t  m_size;

        MappedBlock(uint8_t *ptr, uint64_t size)
            : m_ptr(ptr)
            , m_size(size)
        {}
    };

    class FileMapper
    {
    private:

        uint64_t           m_block_size;
        uint64_t           m_file_size;
        uint64_t           m_blocks_count;

        uint8_t           *m_file_ptr;

        bip::file_mapping  m_file_mapping;
        bip::mapped_region m_mapped_region;

    public:

        FileMapper(const utils::Path& file_name, uint64_t block_size);

        void map();
        void flush();

        [[nodiscard]] uint64_t    getTotalBlocks() const;
        [[nodiscard]] MappedBlock getPtr() const;
        [[nodiscard]] MappedBlock getPtr(uint64_t block_id) const;
    };

    class FileNotMapped final
        : public std::exception
    {
    public:
        [[nodiscard]] const char* what() const override final { return "The file is not mapped yet"; }
    };

    class BlockOutOfRange final
        : public std::exception
    {
    public:
        [[nodiscard]] const char* what() const override final { return "Block number out of range"; }
    };

}