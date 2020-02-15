#pragma once

#include "utils.h"
#include "sha512.h"

namespace builder::crypto
{
    class HashMaker
    {
    private:
        Sha512Context m_context;
        SHA512_HASH   m_digest;
    public:
        HashMaker(uint8_t *data, uint64_t size);

        [[nodiscard]] SHA512_HASH getHash() const;
    };
}