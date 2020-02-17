#pragma once

#include <misc/utils.h>

#include <memory>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>

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

        [[nodiscard]] uint64_t getLimit() const noexcept
        {
            return m_end;
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

        utils::Path                         m_file_name;
        bip::file_mapping                   m_file_mapping;
        std::unique_ptr<bip::mapped_region> m_mapped_region;
        MappingRange                        m_current_range;

    public:
        FileMapper(const utils::Path& file_name, uint64_t block_size);

        void map();
        void flush() const;

        [[nodiscard]] uint64_t    map(uint64_t start_block_id);
        [[nodiscard]] uint64_t    getTotalBlocks() const;
        [[nodiscard]] MappedBlock getPtr(uint64_t block_id) const;

        // TODO: В отдельный класс
        [[nodiscard]] uint8_t     *getRawPtr(uint64_t block_id) const;
    };

    class FileNotMapped final
        : public std::exception
    {
    private:
        mutable std::string m_message;

    public:
        explicit FileNotMapped(const std::string& filename);

        [[nodiscard]] const char *what() const noexcept override final;
    };

    class BlockOutOfRange final
        : public std::exception
    {
    private:
        mutable std::string m_message;

    public:
        explicit BlockOutOfRange(uint64_t block, uint64_t limit);

        [[nodiscard]] const char *what() const noexcept override final;
    };

}