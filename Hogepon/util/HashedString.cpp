#include "HashedString.hpp"
#include <cstring>
#include <cctype>


namespace exlib
{
    HashedString::HashedString(const char* ident_str)
        : m_HashedIdentValue(HashName(ident_str)),
          m_IdentStr(ident_str)
    {}

    HashedString::~HashedString()
    {}

    bool HashedString::operator>(const HashedString& rhs) const noexcept
    {
        return HashValue() > rhs.HashValue();
    }

    bool HashedString::operator<(const HashedString& rhs) const noexcept
    {
        return HashValue() < rhs.HashValue();
    }

    bool HashedString::operator==(const HashedString& rhs) const noexcept
    {
        return HashValue() == rhs.HashValue();
    }

    uint32_t HashedString::HashName(const char* ident_str) noexcept
    {
        // adler-32 checksum
        static const unsigned MOD_ADLER = 65521;

        int len = std::strlen(ident_str);
        const char* str = ident_str;
        unsigned a = 1, b = 0;

        while (len > 0) {
            unsigned tlen = len > 5550 ? 5550 : len;
            len -= tlen;

            do {
                a += std::tolower(*str++);
                b += a;
            } while (--tlen);

            a %= MOD_ADLER;
            b %= MOD_ADLER;
        }

        return (b << 16) | a;
    }


}
