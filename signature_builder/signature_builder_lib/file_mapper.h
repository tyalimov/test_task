#pragma once

#include "utils.h"

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>

#include <memory>

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

    struct MappingRange
    {
    private:
        uint64_t m_start;
        uint64_t m_end;
    
    public:
        MappingRange(uint64_t start, uint64_t end)
            : m_start(start)
            , m_end(end)
        {}

        MappingRange()
            : m_start(0)
            , m_end(0)
        {}

        [[nodiscard]] bool offsetSatisfies(uint64_t offset) const noexcept
        {
            return offset >= m_start && offset <= m_end;
        }
    };

    class FileMapper
    {
    private:
        uint64_t           m_block_size;
        uint64_t           m_file_size;
        uint64_t           m_blocks_count;
        uint64_t           m_page_size;
        uint8_t           *m_file_ptr;

        bip::file_mapping  m_file_mapping;
        //bip::mapped_region m_mapped_region;
        std::unique_ptr<bip::mapped_region> m_mapped_region;
        MappingRange       m_current_range;

    public:
        FileMapper(const utils::Path& file_name, uint64_t block_size);

        void map();
        void flush();

        [[nodiscard]] uint64_t    map(uint64_t start_block_id);
        [[nodiscard]] uint64_t    getTotalBlocks() const;
        [[nodiscard]] MappedBlock getPtr(uint64_t block_id) const;

        // TODO: В отдельный класс
        [[nodiscard]] uint8_t     *getRawPtr(uint64_t block_id) const;
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