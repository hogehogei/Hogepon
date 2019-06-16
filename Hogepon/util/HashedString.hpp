#ifndef   EXLIB_HASHED_STRING_HPP
#define   EXLIB_HASHED_STRING_HPP


#include <string>
#include <stdint.h>


namespace exlib
{

    /**
     * @brief Hashed String
     *
     * 高速な文字列比較、名前から一意のID生成などに用いる
     */
    class HashedString
    {
    public:

        HashedString(const char* ident_str);
        ~HashedString() noexcept;

        bool operator>(const HashedString& rhs) const noexcept;
        bool operator<(const HashedString& rhs) const noexcept;
        bool operator==(const HashedString& rhs) const noexcept;

        //! Get hash value.
        uint32_t HashValue() const noexcept { return m_HashedIdentValue; }
        //! Get string.
        const char* StringBuffer() const noexcept { return m_IdentStr.data(); }

        /**
         * @breif   Adler-32アルゴリズムでハッシュ値生成
         * @return   Hash value.
         */
        static uint32_t HashName(const char* ident_str) noexcept;


    private:

        uint32_t m_HashedIdentValue;
        std::string m_IdentStr;
    };

}

#endif   // EXLIB_HASHED_STRING_HPP
