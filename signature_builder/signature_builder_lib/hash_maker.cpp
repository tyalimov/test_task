#include "hash_maker.h"

namespace builder::crypto
{
    HashMaker::HashMaker(const utils::BinaryBufferPtr& data)
        : m_data(data)
        , m_context()
        , m_digest()
    {
        Sha512Initialise(&m_context);
        Sha512Update(&m_context, data->data(), data->size());
        Sha512Finalise(&m_context, &m_digest);
    }

    // TODO: Подумай, может все-таки unique?
    utils::BinaryBufferPtr HashMaker::getHash() const
    {
        return std::make_shared<utils::BinaryBuffer>(m_digest.bytes, m_digest.bytes + SHA512_DIGEST_SIZE);
    }
}
