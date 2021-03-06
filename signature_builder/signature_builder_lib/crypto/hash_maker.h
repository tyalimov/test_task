﻿#pragma once

#include <misc/utils.h>
#include <third_party/sha512.h>

namespace builder::crypto
{
    using Digest = std::vector<uint8_t>;

    class HashMaker
    {
    private:
        Sha512Context m_context;
        SHA512_HASH   m_digest;
    public:
        HashMaker(uint8_t *data, uint64_t size);

        [[nodiscard]] Digest getHash() const noexcept;
    };
}