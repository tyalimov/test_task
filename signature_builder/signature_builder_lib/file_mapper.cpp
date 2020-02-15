#include "file_mapper.h"

namespace builder::filesys
{
    FileMapper::FileMapper(const utils::Path& file_name, uint64_t block_size)
        : m_block_size(block_size)
        , m_file_size(static_cast<uint64_t>(utils::fs::file_size(file_name)))
        , m_blocks_count(utils::AlignGreater(m_file_size, block_size))
        , m_file_ptr(nullptr)
        , m_file_mapping(file_name.generic_string().c_str(), bip::read_write)
    {}

    void FileMapper::map()
    {
        m_mapped_region = bip::mapped_region
        (
            m_file_mapping, 
            bip::read_write,
            0,
            0
        );

        m_file_ptr = static_cast<uint8_t*>(m_mapped_region.get_address());
    }

    void FileMapper::flush()
    {
        m_mapped_region.flush();
    }

    uint64_t FileMapper::getTotalBlocks() const
    {
        return m_blocks_count;
    }

    MappedBlock FileMapper::getPtr() const
    {
        if (!m_file_ptr)
        {
            throw FileNotMapped();
        }

        return MappedBlock{ m_file_ptr, m_file_size };
    }
   
    MappedBlock FileMapper::getPtr(uint64_t block_id) const
    {
        if (!m_file_ptr)
        {
            throw FileNotMapped();
        }

        if (block_id >= m_blocks_count)
        {
            throw BlockOutOfRange();
        }

        uint8_t *ptr = m_file_ptr + (block_id * m_block_size);

        return block_id == m_blocks_count - 1 
            ? MappedBlock{ ptr, m_file_size - (block_id * m_block_size) }
            : MappedBlock{ ptr, m_block_size };
    }
}
