#ifndef EASYSTL_ALLOCATOR_H
#define EASYSTL_ALLOCATOR_H

#ifndef HEADER_STYLE
#include "utility.h"
#else
#include "../utility.h"
#endif // !HEADER_STYLE

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace easystl {
template <typename Tp> class allocator_base {

  public:
    using value_type = Tp;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using pointer = Tp *;
    using const_pointer = const Tp *;
    using reference = Tp &;
    using const_reference = const Tp &;

    template <typename Tp1> struct rebind {
        typedef allocator_base<Tp1> other;
    };

    using propagate_on_container_move_assignment = std::true_type;

    inline allocator_base() noexcept {}
    inline allocator_base(const allocator_base &) noexcept {}
    template <typename Tp1>
    allocator_base(const allocator_base<Tp1> &) noexcept {}
    allocator_base &operator=(const allocator_base &) = default;
    ~allocator_base() noexcept {}

    pointer address(reference x) const noexcept { return std::addressof(x); }
    const_pointer address(const_reference x) const noexcept {
        return std::addressof(x);
    }

    Tp *allocate(size_type n, const void * = static_cast<const void *>(0)) {
        static_assert(sizeof(Tp) != 0, "cannot allocate incomplete types");

        if (n > this->M_max_size()) {
            if (n > (std::size_t(-1) / sizeof(Tp))) {
                std::__throw_bad_array_new_length();
            }
            std::__throw_bad_alloc();
        }

        return static_cast<Tp *>(::operator new(n * sizeof(Tp)));
    }

    void deallocate(Tp *p, size_type) {
        if (p == nullptr) {
            return;
        }
        ::operator delete(p);
    }

    inline size_type max_size() const noexcept { return M_max_size(); }

    template <class Up, class... Args>
    void construct(Up *p, Args &&...args) noexcept(
        std::is_nothrow_constructible<Up, Args...>::value) {
        ::new ((void *)p) Up(easystl::forward<Args>(args)...);
    }

    template <typename Up>
    inline void
    destroy(Up *p) noexcept(std::is_nothrow_destructible<Up>::value) {
        p->~Up();
    }

    template <typename Up>
    friend inline bool operator==(const allocator_base &,
                                  const allocator_base<Up> &) noexcept {
        return true;
    }
    template <typename Up>
    friend inline bool operator!=(const allocator_base &,
                                  const allocator_base<Up> &) noexcept {
        return false;
    }

  private:
    size_type M_max_size() const noexcept {
#if PTRDIFF_MAX < SIZE_MAX
        return std::size_t(PTRDIFF_MAX) / sizeof(Tp);
#else
        return std::size_t(-1) / sizeof(Tp);
#endif
    }
};
} // namespace easystl

namespace easystl {

template <class Tp> class allocator : public allocator_base<Tp> {
  public:
    using value_type = Tp;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using pointer = Tp *;
    using const_pointer = const Tp *;
    using reference = Tp &;
    using const_reference = const Tp &;

    template <typename Tp1> struct rebind {
        using other = allocator<Tp1>;
    };

    using propagate_on_container_move_assignment = std::true_type;

    using is_always_equal = std::true_type;

  public:
    inline allocator() noexcept {}
    allocator(const allocator &a) noexcept : allocator_base<Tp>(a) {}

    allocator &operator=(const allocator &) = default;

    template <typename Tp1> allocator(const allocator<Tp1> &) noexcept {}
    inline ~allocator() noexcept {}

    friend inline bool operator==(const allocator &,
                                  const allocator &) noexcept {
        return true;
    }
    friend inline bool operator!=(const allocator &,
                                  const allocator &) noexcept {
        return false;
    }
};

template <typename T1, typename T2>
inline bool operator==(const allocator<T1> &, const allocator<T2> &) noexcept {
    return true;
}
template <typename T1, typename T2>
inline bool operator!=(const allocator<T1> &, const allocator<T2> &) noexcept {
    return false;
}

template <typename Tp> class allocator<const Tp> {
  public:
    using value_type = Tp;
    allocator() {}
    template <typename Up> allocator(const allocator<Up> &) {}
};

template <typename Tp> class allocator<volatile Tp> {
  public:
    using value_type = Tp;
    allocator() {}
    template <typename Up> allocator(const allocator<Up> &) {}
};

template <typename Tp> class allocator<const volatile Tp> {
  public:
    using value_type = Tp;
    allocator() {}
    template <typename Up> allocator(const allocator<Up> &) {}
};

template <> class allocator<void> {
  public:
    using value_type = void;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using pointer = void *;
    using const_pointer = const void *;

    template <typename Tp1> struct rebind {
        typedef allocator<Tp1> other;
    };

    using propagate_on_container_move_assignment = std::true_type;

    using is_always_equal = std::true_type;
};

} // namespace easystl

#endif // !EASYSTL_ALLOCATOR_H
