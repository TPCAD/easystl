#ifndef EASYSTL_CHAR_TRAITS_H
#define EASYSTL_CHAR_TRAITS_H

#include "exceptdef.h"
#include <cstddef>
#include <cstring>

namespace easystl {
template <class CharType> struct char_traits {
    typedef CharType char_type;

    static size_t length(const char_type *str) {
        size_t len = 0;
        for (; *str != char_type(0); ++str) {
            ++len;
        }
        return len;
    }

    static int compare(const char_type *str1, const char_type *str2, size_t n) {
        for (; n != 0; --n, ++str1, ++str2) {
            if (*str1 < *str2) {
                return -1;
            }
            if (*str2 < *str1) {
                return 1;
            }
        }
        return 0;
    }

    static char_type *copy(char_type *dest, const char_type *src, size_t n) {
        EASYSTL_DEBUG(src + n <= dest || dest + n <= src);
        char_type *r = dest;
        for (; n != 0; --n, dest++, src++) {
            *dest = *src;
        }
        return r;
    }

    static char_type *move(char_type *dest, const char_type *src, size_t n) {
        char_type *r = dest;
        if (dest < src) {
            for (; n != 0; --n, ++dest, ++src)
                *dest = *src;
        } else if (src < dest) {
            dest += n;
            src += n;
            for (; n != 0; --n)
                *--dest = *--src;
        }
        return r;
    }

    static char_type *fill(char_type *dest, char_type ch, size_t count) {
        char_type *r = dest;
        for (; count > 0; --count, ++dest)
            *dest = ch;
        return r;
    }
};

// partitialize char_traits<char>
template <> struct char_traits<char> {
    typedef char char_type;

    static size_t length(const char_type *str) noexcept {
        return std::strlen(str);
    }

    static int compare(const char_type *str1, const char_type *str2,
                       size_t n) noexcept {
        return std::memcmp(str1, str2, n);
    }

    static char_type *copy(char_type *dest, const char_type *src,
                           size_t n) noexcept {
        EASYSTL_DEBUG(src + n <= dest || dest + n <= src);
        return static_cast<char_type *>(std::memcpy(dest, src, n));
    }

    static char_type *move(char_type *dest, const char_type *src,
                           size_t n) noexcept {
        return static_cast<char_type *>(std::memmove(dest, src, n));
    }

    static char_type *fill(char_type *dest, char_type ch,
                           size_t count) noexcept {
        return static_cast<char_type *>(std::memset(dest, ch, count));
    }
};

// partitialize char_traits<wchar_t>
template <> struct char_traits<wchar_t> {
    typedef wchar_t char_type;

    static size_t length(const char_type *str) noexcept {
        return std::wcslen(str);
    }

    static int compare(const char_type *str1, const char_type *str2,
                       size_t n) noexcept {
        return std::wmemcmp(str1, str2, n);
    }

    static char_type *copy(char_type *dest, const char_type *src,
                           size_t n) noexcept {
        EASYSTL_DEBUG(src + n <= dest || dest + n <= src);
        return static_cast<char_type *>(std::wmemcpy(dest, src, n));
    }

    static char_type *move(char_type *dest, const char_type *src,
                           size_t n) noexcept {
        return static_cast<char_type *>(std::wmemmove(dest, src, n));
    }

    static char_type *fill(char_type *dest, char_type ch,
                           size_t count) noexcept {
        return static_cast<char_type *>(std::wmemset(dest, ch, count));
    }
};

// Partialized. char_traits<char16_t>
// Partialized. char_traits<char32_t>
} // namespace easystl
#endif // !EASYSTL_CHAR_TRAITS_H
