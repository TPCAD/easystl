#ifndef EASYSTL_EXCEPTDEF_H
#define EASYSTL_EXCEPTDEF_H

#include <cassert>
#include <stdexcept>

namespace easystl {

#define EASYSTL_DEBUG(expr) assert(expr)

#define THROW_LENGTH_ERROR_IF(expr, what)                                      \
    if ((expr))                                                                \
    throw std::length_error(what)

#define THROW_OUT_OF_RANGE_IF(expr, what)                                      \
    if ((expr))                                                                \
    throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr, what)                                     \
    if ((expr))                                                                \
    throw std::runtime_error(what)
} // namespace easystl

#endif // !EASYSTL_EXCEPTDEF_H
