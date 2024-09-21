#ifndef EASYSTL_ALLOC_TRAITS_H
#define EASYSTL_ALLOC_TRAITS_H

#include "type_traits.h"
#include "utility.h"
#include <memory>
#include <type_traits>
namespace easystl {

template <typename T> T *to_address(T *p) noexcept { return p; }

template <typename Ptr>
auto to_address(const Ptr &p) noexcept -> decltype(p.operator->()) {
    return p.operator->();
}

template <typename Alloc, typename = typename Alloc::value_type>
struct alloc_traits : std::allocator_traits<Alloc> {
    typedef Alloc allocator_type;

    typedef std::allocator_traits<Alloc> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::difference_type difference_type;

    typedef value_type &reference;
    typedef const value_type &const_reference;
    using base_type::allocate;
    using base_type::construct;
    using base_type::deallocate;
    using base_type::destroy;
    using base_type::max_size;

  private:
    template <typename Ptr>
    using is_custom_pointer = m_bool_constant<std::is_same<pointer, Ptr>() &&
                                              !std::is_pointer<Ptr>()>;

  public:
    // overload construct for non-standard pointer types
    template <typename Ptr, typename... Args>
    static typename std::enable_if<is_custom_pointer<Ptr>::value>::value
    construct(Alloc &a, Ptr p, Args &&...args) noexcept(
        noexcept(base_type::construct(a, easystl::to_address(p),
                                      easystl::forward<Args>(args)...))) {
        base_type::construct(a, easystl::to_address(p),
                             easystl::forward<Args>(args)...);
    }

    // overload destroy for non-standard pointer types
    template <typename Ptr>
    static typename std::enable_if<is_custom_pointer<Ptr>::value>::value
    destroy(Alloc &a, Ptr p) noexcept(
        noexcept(base_type::destroy(a, easystl::to_address(p)))) {
        base_type::destroy(a, easystl::to_address(p));
    }

    static constexpr Alloc S_select_on_copy(const Alloc &a) {
        return base_type::select_on_container_copy_construction(a);
    }

    static void S_on_swap(Alloc &a, Alloc &b) { std::__alloc_on_swap(a, b); }

    static constexpr bool S_propagate_on_copy_assign() {
        return base_type::propagate_on_container_copy_assignment::value;
    }

    static constexpr bool S_propagate_on_move_assign() {
        return base_type::propagate_on_container_move_assignment::value;
    }

    static constexpr bool S_propagate_on_swap() {
        return base_type::propagate_on_container_swap::value;
    }

    static constexpr bool S_always_equal() {
        return base_type::is_always_equal::value;
    }

    static constexpr bool S_nothrow_move() {
        return S_propagate_on_move_assign() || S_always_equal();
    }

    template <typename Tp> struct rebind {
        typedef typename base_type::template rebind_alloc<Tp> other;
    };
};
} // namespace easystl

#endif // !EASYSTL_ALLOC_TRAITS_H
