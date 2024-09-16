#ifndef EASYSTL_CHAR_TRAITS_H
#define EASYSTL_CHAR_TRAITS_H

#include "exceptdef.h"
#include <cstddef>
#include <cstring>
#include <cwchar>
#include <memory.h>
#include <type_traits>

namespace easystl {

template <typename CharType> struct CharTypes {
    typedef unsigned long int_type;
};

template <class CharType> struct char_traits {
    typedef CharType char_type;
    typedef typename CharTypes<CharType>::int_type int_type;

    static void assign(char_type &c1, char_type &c2) { c1 = c2; }

    static bool eq(const char_type &c1, const char_type &c2) {
        return c1 == c2;
    }

    static bool lt(const char_type &c1, const char_type &c2) { return c1 < c2; }

    static int compare(const char_type *str1, const char_type *str2, size_t n);

    static size_t length(const char_type *str);

    static const char_type *find(const char_type *s, std::size_t n,
                                 const char_type &c);

    static char_type *move(char_type *dest, const char_type *src, size_t n);

    static char_type *copy(char_type *dest, const char_type *src, size_t n);

    static char_type *assign(char_type *s, std::size_t n, char_type &c);

    static char_type to_char_type(const int_type &c) {
        return static_cast<char_type>(c);
    }

    static char_type to_int_type(const char_type &c) {
        return static_cast<int_type>(c);
    }

    static bool eq_int_type(const int_type &c1, const int_type &c2) {
        return c1 == c2;
    }

    static char_type *fill(char_type *dest, char_type ch, size_t count) {
        char_type *r = dest;
        for (; count > 0; --count, ++dest)
            *dest = ch;
        return r;
    }
};

template <typename CharType>
int char_traits<CharType>::compare(const char_type *str1, const char_type *str2,
                                   size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        if (lt(str1[i], str2[i])) {
            return -1;
        } else if (lt(str2[i], str1[i])) {
            return 1;
        }
    }
    return 0;
}

template <typename CharType>
std::size_t char_traits<CharType>::length(const char_type *str) {
    size_t len = 0;
    while (!eq(str[len], char_type())) {
        ++len;
    }
    return len;
}

template <typename CharType>
const typename char_traits<CharType>::char_type *
char_traits<CharType>::find(const char_type *s, std::size_t n,
                            const char_type &c) {
    for (std::size_t i = 0; i < n; ++i) {
        if (eq(s[i], c)) {
            return s + i;
        }
    }
    return 0;
}
template <typename CharType>
typename char_traits<CharType>::char_type *
char_traits<CharType>::move(char_type *dest, const char_type *src, size_t n) {
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
template <typename CharType>
typename char_traits<CharType>::char_type *
char_traits<CharType>::copy(char_type *dest, const char_type *src, size_t n) {
    EASYSTL_DEBUG(src + n <= dest || dest + n <= src);
    char_type *r = dest;
    for (; n != 0; --n, dest++, src++) {
        *dest = *src;
    }
    return r;
}

template <typename CharType>
typename char_traits<CharType>::char_type *
char_traits<CharType>::assign(char_type *s, std::size_t n, char_type &c) {
    for (std::size_t i = 0; i < n; ++i) {
        s[i] = c;
    }
    return s;
}

// partitialize char_traits<char>
template <> struct char_traits<char> {
    typedef char char_type;
    typedef int int_type;

    static void assign(char_type &c1, const char_type &c2) noexcept { c1 = c2; }

    static bool eq(const char_type &c1, const char_type &c2) noexcept {
        return c1 == c2;
    }

    static bool lt(const char_type &c1, const char_type &c2) noexcept {
        return (static_cast<unsigned char>(c1) <
                static_cast<unsigned char>(c2));
    }

    static int compare(const char_type *str1, const char_type *str2, size_t n) {
        if (n == 0) {
            return 0;
        }
        return std::memcmp(str1, str2, n);
    }

    static size_t length(const char_type *str) noexcept {
        return std::strlen(str);
    }

    static const char_type *find(const char_type *s, std::size_t n,
                                 const char_type &c) {

        if (n == 0) {
            return 0;
        }
        return static_cast<const char_type *>(std::memchr(s, n, c));
    }

    static char_type *move(char_type *dest, const char_type *src, size_t n) {
        if (n == 0) {
            return dest;
        }
        return static_cast<char_type *>(std::memmove(dest, src, n));
    }

    static char_type *copy(char_type *dest, const char_type *src,
                           size_t n) noexcept {
        if (n == 0) {
            return dest;
        }
        // EASYSTL_DEBUG(src + n <= dest || dest + n <= src);
        return static_cast<char_type *>(std::memcpy(dest, src, n));
    }

    static char_type *assign(char_type *dest, std::size_t n,
                             char_type c) noexcept {

        if (n == 0) {
            return dest;
        }
        return static_cast<char_type *>(std::memset(dest, c, n));
    }

    static char_type to_char_type(const int_type &c) noexcept {
        return static_cast<char_type>(c);
    }

    static char_type to_int_type(const char_type &c) noexcept {
        return static_cast<int_type>(c);
    }

    static bool eq_int_type(const int_type &c1, const int_type &c2) {
        return c1 == c2;
    }

    static char_type *fill(char_type *dest, char_type ch,
                           size_t count) noexcept {
        return static_cast<char_type *>(std::memset(dest, ch, count));
    }
};

// partitialize char_traits<wchar_t>
// template <> struct char_traits<wchar_t> {
//     typedef wchar_t char_type;
//
//     static size_t length(const char_type *str) noexcept {
//         return std::wcslen(str);
//     }
//
//     static int compare(const char_type *str1, const char_type *str2,
//                        size_t n) noexcept {
//         return std::wmemcmp(str1, str2, n);
//     }
//
//     static char_type *copy(char_type *dest, const char_type *src,
//                            size_t n) noexcept {
//         EASYSTL_DEBUG(src + n <= dest || dest + n <= src);
//         return static_cast<char_type *>(std::wmemcpy(dest, src, n));
//     }
//
//     static char_type *move(char_type *dest, const char_type *src,
//                            size_t n) noexcept {
//         return static_cast<char_type *>(std::wmemmove(dest, src, n));
//     }
//
//     static char_type *fill(char_type *dest, char_type ch,
//                            size_t count) noexcept {
//         return static_cast<char_type *>(std::wmemset(dest, ch, count));
//     }
// };

// Partialized. char_traits<char16_t>
// Partialized. char_traits<char32_t>
} // namespace easystl
#endif // !EASYSTL_CHAR_TRAITS_H
