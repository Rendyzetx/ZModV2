

#pragma once

#include <cstddef>

namespace xs_detail {

template <typename CharT>
constexpr CharT XsMask(unsigned key, std::size_t i) {
    return static_cast<CharT>(((key + i * 31u) ^ 0xCAFEu) & 0xFFFFu);
}

template <typename CharT, std::size_t N, unsigned Key>
class XorStr {
    CharT         enc_[N];
    mutable CharT buf_[N];

public:
    constexpr XorStr(const CharT (&s)[N]) : enc_{}, buf_{} {
        for (std::size_t i = 0; i < N; ++i) {
            enc_[i] = static_cast<CharT>(s[i] ^ XsMask<CharT>(Key, i));
            buf_[i] = static_cast<CharT>(0);
        }
    }

    const CharT* c_str() const {
        for (std::size_t i = 0; i < N; ++i) {
            buf_[i] = static_cast<CharT>(enc_[i] ^ XsMask<CharT>(Key, i));
        }
        return buf_;
    }
};

constexpr unsigned XsKey(unsigned line, unsigned counter) {
    return ((line * 2654435761u) ^ (counter * 40503u)) ^ 0xDEADBEEFu;
}

}

#define XS(s)  (::xs_detail::XorStr<char, sizeof(s), \
    ::xs_detail::XsKey(__LINE__, __COUNTER__)>(s).c_str())

#define XSW(s) (::xs_detail::XorStr<wchar_t, sizeof(s) / sizeof(wchar_t), \
    ::xs_detail::XsKey(__LINE__, __COUNTER__)>(s).c_str())
