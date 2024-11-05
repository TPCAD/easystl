#ifndef EASYSTL_MEMORY_H
#define EASYSTL_MEMORY_H

#include <memory/alloc_traits.h>
#include <memory/allocator.h>
// #include "uninitialized.h"

namespace easystl {

template <class Tp> constexpr Tp *address_of(Tp &value) noexcept {
    return &value;
}

} // namespace easystl

#endif // !EASYSTL_MEMORY_H
