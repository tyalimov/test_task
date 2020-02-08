#pragma once

#include "utils.h"
#include "sha512.h"

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

        [[nodiscard]] utils::BinaryBufferPtr getHash() const;
    };
}