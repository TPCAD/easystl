#ifndef EASYSTL_UTILITY_H
#define EASYSTL_UTILITY_H

#include <type_traits>

namespace easystl {

// move
template <class T>
typename std::remove_reference<T>::type &&move(T &&arg) noexcept {
    return static_cast<typename std::remove_reference<T>::type &&>(arg);
}

// forward
template <class T>
T &&forward(typename std::remove_reference<T>::type &arg) noexcept {
    return static_cast<T &&>(arg);
}

template <class T>
T &&forward(typename std::remove_reference<T>::type &&arg) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
    return static_cast<T &&>(arg);
}

// swap
template <class Tp> void swap(Tp &lhs, Tp &rhs) {
    auto tmp = easystl::move(lhs);
    lhs = easystl::move(rhs);
    rhs = easystl::move(tmp);
}

// pair

// default constructible
template <class Ty1, class Ty2> struct pair {
    typedef Ty1 first_type;
    typedef Ty2 second_type;

    first_type first;
    second_type second;

    template <class Other1 = Ty1, class Other2 = Ty2,
              typename = typename std::enable_if<
                  std::is_default_constructible<Other1>::value &&
                      std::is_default_constructible<Other2>::value,
                  void>::value>
    constexpr pair() : first(), second() {}

    // implicit constructible for this type
    template <class U1 = Ty1, class U2 = Ty2,
              typename std::enable_if<
                  std::is_copy_constructible<U1>::value &&
                      std::is_copy_constructible<U2>::value &&
                      std::is_convertible<const U1 &, Ty1>::value &&
                      std::is_convertible<const U2 &, Ty2>::value,
                  int>::type = 0>
    constexpr pair(const Ty1 &a, const Ty2 &b) : first(a), second(b) {}

    template <class U1 = Ty1, class U2 = Ty2,
              typename std::enable_if<
                  std::is_copy_constructible<U1>::value &&
                      std::is_copy_constructible<U2>::value &&
                      (!std::is_convertible<const U1 &, Ty1>::value ||
                       !std::is_convertible<const U2 &, Ty2>::value),
                  int>::type = 0>
    explicit constexpr pair(const Ty1 &a, const Ty2 &b) : first(a), second(b) {}
};

} // namespace easystl

#endif // !EASYSTL_UTILITY_H
