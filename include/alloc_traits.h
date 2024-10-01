#ifndef EASYSTL_ALLOC_TRAITS_H
#define EASYSTL_ALLOC_TRAITS_H

#include "allocator.h"
#include "type_traits.h"
#include "utility.h"

namespace easystl {
struct allocator_traits_base {
    template <typename Tp, typename Up, typename = void>
    struct rebind : std::__replace_first_arg<Tp, Up> {
        static_assert(
            std::is_same<typename std::__replace_first_arg<
                             Tp, typename Tp::value_type>::type,
                         Tp>::value,
            "allocator_traits<A>::rebind_alloc<A::value_type> must be A");
    };

    template <typename Tp, typename Up>
    struct rebind<Tp, Up,
                  std::__void_t<typename Tp::template rebind<Up>::other>> {
        using type = typename Tp::template rebind<Up>::other;

        static_assert(
            std::is_same<
                typename Tp::template rebind<typename Tp::value_type>::other,
                Tp>::value,
            "allocator_traits<A>::rebind_alloc<A::value_type> must be A");
    };

  protected:
    template <typename Tp> using __pointer = typename Tp::pointer;
    template <typename Tp> using __c_pointer = typename Tp::const_pointer;
    template <typename Tp> using __v_pointer = typename Tp::void_pointer;
    template <typename Tp> using __cv_pointer = typename Tp::const_void_pointer;
    template <typename Tp>
    using __pocca = typename Tp::propagate_on_container_copy_assignment;
    template <typename Tp>
    using __pocma = typename Tp::propagate_on_container_move_assignment;
    template <typename Tp>
    using __pocs = typename Tp::propagate_on_container_swap;
    template <typename Tp>
    using __equal = std::__type_identity<typename Tp::is_always_equal>;
};

template <typename Alloc, typename Up>
using alloc_rebind =
    typename allocator_traits_base::template rebind<Alloc, Up>::type;

template <typename Alloc> struct allocator_traits : allocator_traits_base {
    // 分配器类型
    typedef Alloc allocator_type;
    // 被分配的类型
    typedef typename Alloc::value_type value_type;

    // 指针类型
    using pointer = std::__detected_or_t<value_type *, __pointer, Alloc>;

  private:
    template <template <typename> class Func, typename Tp, typename = void>
    struct Ptr {
        using type = typename std::pointer_traits<pointer>::template rebind<Tp>;
    };

    template <template <typename> class Func, typename Tp>
    struct Ptr<Func, Tp, std::__void_t<Func<Alloc>>> {
        using type = Func<Alloc>;
    };

    template <typename A2, typename PtrT, typename = void> struct Diff {
        using type = typename std::pointer_traits<PtrT>::difference_type;
    };

    template <typename A2, typename PtrT>
    struct Diff<A2, PtrT, std::__void_t<typename A2::difference_type>> {
        using type = typename A2::difference_type;
    };

    template <typename A2, typename DiffT, typename = void>
    struct Size : std::make_unsigned<DiffT> {};

    template <typename A2, typename DiffT>
    struct Size<A2, DiffT, std::__void_t<typename A2::size_type>> {
        using type = typename A2::size_type;
    };

  public:
    using const_pointer = typename Ptr<__c_pointer, const value_type>::type;

    using void_pointer = typename Ptr<__v_pointer, void>::type;

    using const_void_pointer = typename Ptr<__cv_pointer, const void>::type;

    using difference_type = typename Diff<Alloc, pointer>::type;

    using size_type = typename Size<Alloc, difference_type>::type;

    using propagate_on_container_copy_assignment =
        std::__detected_or_t<std::false_type, __pocca, Alloc>;

    using propagate_on_container_move_assignment =
        std::__detected_or_t<std::false_type, __pocma, Alloc>;

    using propagate_on_container_swap =
        std::__detected_or_t<std::false_type, __pocs, Alloc>;

    using is_always_equal = typename std::__detected_or_t<std::is_empty<Alloc>,
                                                          __equal, Alloc>::type;

    template <typename Tp> using rebind_alloc = alloc_rebind<Alloc, Tp>;
    template <typename Tp>
    using rebind_traits = allocator_traits<rebind_alloc<Tp>>;

  private:
    template <typename Alloc2>
    static constexpr auto S_allocate(Alloc2 &a, size_type n,
                                     const_void_pointer hint,
                                     int) -> decltype(a.allocate(n, hint)) {
        return a.allocate(n, hint);
    }

    template <typename Alloc2>
    static constexpr pointer S_allocate(Alloc2 &a, size_type n,
                                        const_void_pointer, ...) {
        return a.allocate(n);
    }

    // 检测是否支持 construct
    template <typename Tp, typename... Args> struct construct_helper {
        template <typename Alloc2,
                  typename = decltype(std::declval<Alloc2 *>()->construct(
                      std::declval<Tp *>(), std::declval<Args>()...))>
        static std::true_type test(int);

        template <typename> static std::false_type __test(...);

        using type = decltype(test<Alloc>(0));
    };

    template <typename Tp, typename... Args>
    using has_construct = typename construct_helper<Tp, Args...>::type;

    template <typename Tp, typename... Args>
    static std::_Require<has_construct<Tp, Args...>>
    S_construct(Alloc &a, Tp *p, Args &&...args) noexcept(
        noexcept(a.construct(p, std::forward<Args>(args)...))) {
        a.construct(p, std::forward<Args>(args)...);
    }

    // 不支持 cunstruct 操作，但是 constructible
    template <typename Tp, typename... Args>
    static std::_Require<std::__and_<std::__not_<has_construct<Tp, Args...>>,
                                     std::is_constructible<Tp, Args...>>>
    S_construct(Alloc &, Tp *p, Args &&...args) noexcept(
        std::is_nothrow_constructible<Tp, Args...>::value) {
        ::new ((void *)p) Tp(std::forward<Args>(args)...);
    }

    template <typename Alloc2, typename Tp>
    static auto
    S_destroy(Alloc2 &a, Tp *p,
              int) noexcept(noexcept(a.destroy(p))) -> decltype(a.destroy(p)) {
        a.destroy(p);
    }

    template <typename Alloc2, typename Tp>
    static void
    S_destroy(Alloc2 &, Tp *p,
              ...) noexcept(std::is_nothrow_destructible<Tp>::value) {
        std::_Destroy(p);
    }

    template <typename Alloc2>
    static constexpr auto S_max_size(Alloc2 &a, int) -> decltype(a.max_size()) {
        return a.max_size();
    }

    template <typename Alloc2>
    static constexpr size_type S_max_size(Alloc2 &, ...) {
        return __gnu_cxx::__numeric_traits<size_type>::__max /
               sizeof(value_type);
    }

    template <typename Alloc2>
    static constexpr auto S_select(Alloc2 &a, int)
        -> decltype(a.select_on_container_copy_construction()) {
        return a.select_on_container_copy_construction();
    }

    template <typename Alloc2>
    static constexpr Alloc2 S_select(Alloc2 &a, ...) {
        return a;
    }

  public:
    static pointer allocate(Alloc &a, size_type n) { return a.allocate(n); }
    static pointer allocate(Alloc &a, size_type n, const_void_pointer hint) {
        return S_allocate(a, n, hint, 0);
    }

    static void deallocate(Alloc &a, pointer p, size_type n) {
        a.deallocate(p, n);
    }

    template <typename Tp, typename... Args>
    static auto construct(Alloc &a, Tp *p, Args &&...args) noexcept(
        noexcept(S_construct(a, p, std::forward<Args>(args)...)))
        -> decltype(S_construct(a, p, std::forward<Args>(args)...)) {
        S_construct(a, p, std::forward<Args>(args)...);
    }

    template <typename Tp>
    static void destroy(Alloc &a,
                        Tp *p) noexcept(noexcept(S_destroy(a, p, 0))) {
        S_destroy(a, p, 0);
    }

    static size_type max_size(const Alloc &a) noexcept {
        return S_max_size(a, 0);
    }

    static Alloc select_on_container_copy_construction(const Alloc &rhs) {
        return S_select(rhs, 0);
    }
};

template <typename Tp> struct allocator_traits<allocator<Tp>> {
    /// The allocator type
    using allocator_type = allocator<Tp>;

    /// The allocated type
    using value_type = Tp;

    /// The allocator's pointer type.
    using pointer = Tp *;

    /// The allocator's const pointer type.
    using const_pointer = const Tp *;

    /// The allocator's void pointer type.
    using void_pointer = void *;

    /// The allocator's const void pointer type.
    using const_void_pointer = const void *;

    /// The allocator's difference type
    using difference_type = std::ptrdiff_t;

    /// The allocator's size type
    using size_type = std::size_t;

    /// How the allocator is propagated on copy assignment
    using propagate_on_container_copy_assignment = std::false_type;

    /// How the allocator is propagated on move assignment
    using propagate_on_container_move_assignment = std::true_type;

    /// How the allocator is propagated on swap
    using propagate_on_container_swap = std::false_type;

    /// Whether all instances of the allocator type compare equal.
    using is_always_equal = std::true_type;

    template <typename Up> using rebind_alloc = allocator<Up>;

    template <typename Up>
    using rebind_traits = allocator_traits<allocator<Up>>;

    inline static pointer allocate(allocator_type &a, size_type n) {
        return a.allocate(n);
    }

    inline static pointer allocate(allocator_type &a, size_type n,
                                   const_void_pointer hint) {
        return a.allocate(n, hint);
    };

    inline static void deallocate(allocator_type &a, pointer p, size_type n) {
        a.deallocate(p, n);
    }

    template <typename Up, typename... Args>
    inline static void
    construct(allocator_type &a, Up *p, Args &&...args) noexcept(
        std::is_nothrow_constructible<Up, Args...>::value) {
        a.construct(p, std::forward<Args>(args)...);
    }

    template <typename Up>
    inline static void
    destroy(allocator_type &a,
            Up *p) noexcept(std::is_nothrow_destructible<Up>::value) {
        a.destroy(p);
    }

    inline static size_type max_size(const allocator_type &a) noexcept {
        return a.max_size();
    }

    inline static allocator_type
    select_on_container_copy_construction(const allocator_type &rhs) {
        return rhs;
    }
};

template <> struct allocator_traits<allocator<void>> {
    /// The allocator type
    using allocator_type = allocator<void>;

    /// The allocated type
    using value_type = void;

    /// The allocator's pointer type.
    using pointer = void *;

    /// The allocator's const pointer type.
    using const_pointer = const void *;

    /// The allocator's void pointer type.
    using void_pointer = void *;

    /// The allocator's const void pointer type.
    using const_void_pointer = const void *;

    /// The allocator's difference type
    using difference_type = std::ptrdiff_t;

    /// The allocator's size type
    using size_type = std::size_t;

    /// How the allocator is propagated on copy assignment
    using propagate_on_container_copy_assignment = std::false_type;

    /// How the allocator is propagated on move assignment
    using propagate_on_container_move_assignment = std::true_type;

    /// How the allocator is propagated on swap
    using propagate_on_container_swap = std::false_type;

    /// Whether all instances of the allocator type compare equal.
    using is_always_equal = std::true_type;

    template <typename Up> using rebind_alloc = allocator<Up>;

    template <typename Up>
    using rebind_traits = allocator_traits<allocator<Up>>;

    /// allocate is ill-formed for allocator<void>
    static void *allocate(allocator_type &, size_type,
                          const void * = nullptr) = delete;

    /// deallocate is ill-formed for allocator<void>
    static void deallocate(allocator_type &, void *, size_type) = delete;

    template <typename Up, typename... Args>
    inline static void
    construct(allocator_type &, Up *p, Args &&...args) noexcept(
        std::is_nothrow_constructible<Up, Args...>::value) {
        std::_Construct(p, std::forward<Args>(args)...);
    }

    template <typename Up>
    inline static void
    destroy(allocator_type &,
            Up *p) noexcept(std::is_nothrow_destructible<Up>::value) {
        std::_Destroy(p);
    }

    /// max_size is ill-formed for allocator<void>
    static size_type max_size(const allocator_type &) = delete;

    inline static allocator_type
    select_on_container_copy_construction(const allocator_type *rhs) {
        return *rhs;
    }
};

template <typename T> T *to_address(T *p) noexcept { return p; }

template <typename Ptr>
auto to_address(const Ptr &p) noexcept -> decltype(p.operator->()) {
    return p.operator->();
}

} // namespace easystl

namespace easystl_cxx {

template <typename Alloc, typename = typename Alloc::value_type>
struct alloc_traits : easystl::allocator_traits<Alloc> {
    typedef Alloc allocator_type;

    typedef easystl::allocator_traits<Alloc> base_type;
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
    using is_custom_pointer =
        easystl::m_bool_constant<std::is_same<pointer, Ptr>() &&
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

} // namespace easystl_cxx

#endif // !EASYSTL_ALLOC_TRAITS_H
