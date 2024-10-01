#ifndef EASYSTL_ALLOCATOR_H
#define EASYSTL_ALLOCATOR_H

#include "utility.h"
#include <cstddef>
#include <memory.h>
#include <stdexcept>
#include <type_traits>

namespace easystl {
template <typename Tp> class allocator_base {

  public:
    typedef Tp value_type;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    typedef Tp *pointer;
    typedef const Tp *const_pointer;
    typedef Tp &reference;
    typedef const Tp &const_reference;

    template <typename Tp1> struct rebind {
        typedef allocator_base<Tp1> other;
    };

    typedef std::true_type propagate_on_container_move_assignment;

    inline allocator_base() noexcept {}
    inline allocator_base(const allocator_base &) noexcept {}
    template <typename Tp1>
    allocator_base(const allocator_base<Tp1> &) noexcept {}
    allocator_base &operator=(const allocator_base &) = default;
    ~allocator_base() noexcept {}

    pointer address(reference x) const noexcept { return std::__addressof(x); }
    const_pointer address(const_reference x) const noexcept {
        return std::__addressof(x);
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
#if __PTRDIFF_MAX__ < __SIZE_MAX__
        return std::size_t(__PTRDIFF_MAX__) / sizeof(Tp);
#else
        return std::size_t(-1) / sizeof(_Tp);
#endif
    }
};
} // namespace easystl

namespace easystl {

template <class Tp> class allocator : public allocator_base<Tp> {
  public:
    typedef Tp value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef Tp *pointer;
    typedef const Tp *const_pointer;
    typedef Tp &reference;
    typedef const Tp &const_reference;

    template <typename Tp1> struct rebind {
        typedef allocator<Tp1> other;
    };

    using propagate_on_container_move_assignment = std::true_type;

    using is_always_equal _GLIBCXX20_DEPRECATED_SUGGEST(
        "std::allocator_traits::is_always_equal") = std::true_type;

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
    typedef Tp value_type;
    allocator() {}
    template <typename Up> allocator(const allocator<Up> &) {}
};

template <typename Tp> class allocator<volatile Tp> {
  public:
    typedef Tp value_type;
    allocator() {}
    template <typename Up> allocator(const allocator<Up> &) {}
};

template <typename Tp> class allocator<const volatile Tp> {
  public:
    typedef Tp value_type;
    allocator() {}
    template <typename Up> allocator(const allocator<Up> &) {}
};

template <> class allocator<void> {
  public:
    typedef void value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef void *pointer;
    typedef const void *const_pointer;

    template <typename Tp1> struct rebind {
        typedef allocator<Tp1> other;
    };

    using propagate_on_container_move_assignment = std::true_type;

    using is_always_equal = std::true_type;
};

} // namespace easystl

#endif // !EASYSTL_ALLOCATOR_H
