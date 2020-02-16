#include "file_mapper.h"

constexpr uint64_t k1Gb{ 1024 * 1024 * 1024 };

namespace builder::filesys
{
    FileMapper::FileMapper(const utils::Path& file_name, uint64_t block_size)
        : m_block_size(block_size)
        , m_file_size(static_cast<uint64_t>(utils::fs::file_size(file_name)))
        , m_blocks_count(utils::AlignGreater(m_file_size, block_size))
        , m_page_size(bip::mapped_region::get_page_size())
        , m_file_ptr(nullptr)
        , m_file_mapping(file_name.generic_string().c_str(), bip::read_write)
    {}

    void FileMapper::map()
    {
        //flush();

        m_mapped_region.reset();

        m_mapped_region = std::make_unique<bip::mapped_region>
        (
            m_file_mapping, 
            bip::read_write,
            0,
            0
        );

        m_file_ptr      = static_cast<uint8_t*>(m_mapped_region->get_address());
        m_current_range = MappingRange{ 0, m_file_size - 1 };
    }

    // returns available blocks count
    uint64_t FileMapper::map(uint64_t start_block_id)
    {
        uint64_t page_number  = (start_block_id * m_block_size) / m_page_size;
        uint64_t page_offset = page_number * m_page_size;
        uint64_t mapping_size = m_file_size - page_offset > k1Gb
            ? k1Gb
            : m_file_size - page_offset;

        uint64_t offset_from_nearest_page = start_block_id * m_block_size - page_offset;

        uint64_t available_blocks = utils::AlignGreater(mapping_size - offset_from_nearest_page, m_block_size);

        //flush();

        m_mapped_region.reset();

        m_mapped_region = std::make_unique<bip::mapped_region>
        (
            m_file_mapping, 
            bip::read_write,
            page_offset,
            static_cast<size_t>(mapping_size)
        );

        m_file_ptr      = static_cast<uint8_t*>(m_mapped_region->get_address());
        m_current_range = MappingRange{ page_offset, page_offset + mapping_size - 1 };

        return available_blocks;
    }

    void FileMapper::flush()
    {
        m_mapped_region->flush();
    }

    uint64_t FileMapper::getTotalBlocks() const
    {
        return m_blocks_count;
    }

    MappedBlock FileMapper::getPtr(uint64_t block_id) const
    {
        if (!m_file_ptr)
        {
            throw FileNotMapped();
        }

        uint64_t requested_offser = m_block_size * block_id;

        if (!m_current_range.offsetSatisfies(requested_offser))
        {
            throw BlockOutOfRange();
        }

        uint64_t offset_from_nearest_page = requested_offser % m_page_size;
        uint8_t  *ptr                     = m_file_ptr + (requested_offser / m_page_size) + offset_from_nearest_page;

        return block_id == m_blocks_count - 1 
            ? MappedBlock{ ptr, m_file_size - (block_id * m_block_size) }
            : MappedBlock{ ptr, m_block_size };
    }

    uint8_t * FileMapper::getRawPtr(uint64_t block_id) const
    {
        return m_file_ptr + block_id * m_block_size;
    }
}
