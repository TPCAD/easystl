#ifndef EASYSTL_EXCEPTDEF_H
#define EASYSTL_EXCEPTDEF_H

#include <cassert>
#include <stdexcept>

namespace easystl {

#define EASYSTL_DEBUG(expr) assert(expr)

#define THROW_LOGIC_ERROR_IF(expr, what)                                       \
    if ((expr))                                                                \
    throw std::logic_error(what)

#define THROW_LENGTH_ERROR_IF(expr, what)                                      \
    if ((expr))                                                                \
    throw std::length_error(what)

#define THROW_OUT_OF_RANGE_IF(expr, what)                                      \
    if ((expr))                                                                \
    throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr, what)                                     \
    if ((expr))                                                                \
    throw std::runtime_error(what)

#define easystl_require_string(string) assert((string) != nullptr)
#define easystl_require_string_len(string, len)                                \
    assert((string) != nullptr || (len) == 0)
} // namespace easystl

#endif // !EASYSTL_EXCEPTDEF_H
