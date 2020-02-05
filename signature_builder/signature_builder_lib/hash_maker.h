#pragma once

#include "utils.h"
#include "sha512.h"

// TODO: Заменить string на vector<uit8_t>

namespace builder::crypto
{
    class HashMaker
    {
    private:
        utils::BinaryBufferPtr  m_data;
        Sha512Context           m_context;
        SHA512_HASH             m_digest;
    public:
        explicit HashMaker(const utils::BinaryBufferPtr& data);

        utils::BinaryBufferPtr getHash() const;
    };
}