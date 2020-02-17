﻿#include "hash_maker.h"

namespace builder::crypto
{
    HashMaker::HashMaker(uint8_t *data, uint64_t size)
        : m_context()
        , m_digest()
    {
        Sha512Initialise(&m_context);
        Sha512Update(&m_context, data, static_cast<uint32_t>(size));
        Sha512Finalise(&m_context, &m_digest);
    }

    std::vector<uint8_t> HashMaker::getHash() const noexcept
    {
        return std::vector<uint8_t>(m_digest.bytes, m_digest.bytes + sizeof(m_digest.bytes));
    }
}
