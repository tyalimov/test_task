#include "hash_maker.h"

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

    SHA512_HASH HashMaker::getHash() const
    {
        return m_digest;
    }
}
