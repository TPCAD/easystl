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

template <class Ty1, class Ty2> struct pair {
    typedef Ty1 first_type;
    typedef Ty2 second_type;

    first_type first;
    second_type second;

    // default constructible
    template <class Other1 = Ty1, class Other2 = Ty2,
              typename = typename std::enable_if<
                  std::is_default_constructible<Other1>::value &&
                      std::is_default_constructible<Other2>::value,
                  void>::type>
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

    // explicit constructible for this type
    template <class U1 = Ty1, class U2 = Ty2,
              typename std::enable_if<
                  std::is_copy_constructible<U1>::value &&
                      std::is_copy_constructible<U2>::value &&
                      (!std::is_convertible<const U1 &, Ty1>::value ||
                       !std::is_convertible<const U2 &, Ty2>::value),
                  int>::type = 0>
    explicit constexpr pair(const Ty1 &a, const Ty2 &b) : first(a), second(b) {}

    // implicit constructible for other type
    template <class Other1, class Other2,
              typename std::enable_if<
                  std::is_constructible<Ty1, Other1>::value &&
                      std::is_constructible<Ty2, Other2>::value &&
                      std::is_convertible<Other1 &&, Ty1>::value &&
                      std::is_convertible<Other2 &&, Ty2>::value,
                  int>::type = 0>
    constexpr pair(Other1 &&a, Other2 &&b)
        : first(easystl::forward<Other1>(a)),
          second(easystl::forward<Other2>(b)) {}

    // explicit constructible for other type
    template <
        class Other1, class Other2,
        typename std::enable_if<std::is_constructible<Ty1, Other1>::value &&
                                    std::is_constructible<Ty2, Other2>::value &&
                                    (!std::is_convertible<Other1, Ty1>::value ||
                                     !std::is_convertible<Other2, Ty2>::value),
                                int>::type = 0>
    explicit constexpr pair(Other1 &&a, Other2 &&b)
        : first(easystl::forward<Other1>(a)),
          second(easystl::forward<Other2>(b)) {}

    pair(const pair &rhs) = default;
    pair(pair &&rhs) = default;

    // implicit constructible for other pair
    template <class Other1, class Other2,
              typename std::enable_if<
                  std::is_constructible<Ty1, const Other1 &>::value &&
                      std::is_constructible<Ty2, const Other2 &>::value &&
                      std::is_convertible<const Other1 &, Ty1>::value &&
                      std::is_convertible<const Other2 &, Ty2>::value,
                  int>::type = 0>
    constexpr pair(const pair<Other1, Other2> &other)
        : first(other.first), second(other.second) {}

    // explicit constructible for other pair
    template <class Other1, class Other2,
              typename std::enable_if<
                  std::is_constructible<Ty1, const Other1 &>::value &&
                      std::is_constructible<Ty2, const Other2 &>::value &&
                      (!std::is_convertible<const Other1 &, Ty1>::value ||
                       !std::is_convertible<const Other2 &, Ty2>::value),
                  int>::type = 0>
    explicit constexpr pair(const pair<Other1, Other2> &other)
        : first(other.first), second(other.second) {}

    // implicit constructible for other pair
    template <
        class Other1, class Other2,
        typename std::enable_if<std::is_constructible<Ty1, Other1>::value &&
                                    std::is_constructible<Ty2, Other2>::value &&
                                    std::is_convertible<Other1, Ty1>::value &&
                                    std::is_convertible<Other2, Ty2>::value,
                                int>::type = 0>
    constexpr pair(pair<Other1, Other2> &&other)
        : first(easystl::forward<Other1>(other.first)),
          second(easystl::forward<Other2>(other.second)) {}

    // explicit constructible for other pair
    template <
        class Other1, class Other2,
        typename std::enable_if<std::is_constructible<Ty1, Other1>::value &&
                                    std::is_constructible<Ty2, Other2>::value &&
                                    (!std::is_convertible<Other1, Ty1>::value ||
                                     !std::is_convertible<Other2, Ty2>::value),
                                int>::type = 0>
    explicit constexpr pair(pair<Other1, Other2> &&other)
        : first(easystl::forward<Other1>(other.first)),
          second(easystl::forward<Other2>(other.second)) {}

    // copy asign for this pair
    pair &
    operator=(typename std::enable_if<std::is_copy_assignable<Ty1>::value &&
                                          std::is_copy_assignable<Ty2>::value,
                                      const pair &>::type rhs) {
        if (this != &rhs) {
            first = rhs.first;
            second = rhs.second;
        }
        return *this;
    }

    // move asign for this pair
    pair &
    operator=(typename std::enable_if<std::is_copy_assignable<Ty1>::value &&
                                          std::is_copy_assignable<Ty2>::value,
                                      const pair &&>::type rhs) {
        if (this != &rhs) {
            first = easystl::move(rhs.first);
            second = easystl::move(rhs.second);
        }
        return *this;
    }

    // copy asign for other pair
    template <class Other1, class Other2>
    typename std::enable_if<
        std::is_assignable<Ty1 &, const Other1 &>::value &&
            std::is_assignable<Ty2 &, const Other2 &>::value,
        pair &>::type
    operator=(const pair<Other1, Other2> &other) {
        first = other.first;
        second = other.second;
        return *this;
    }

    // move asign for other pair
    template <class Other1, class Other2>
    typename std::enable_if<std::is_assignable<Ty1 &, Other1 &&>::value &&
                                std::is_assignable<Ty2 &, Other2 &&>::value,
                            pair &>::type
    operator=(const pair<Other1, Other2> &&other) {
        first = easystl::forward(other.first);
        second = easystl::forward(other.second);
        return *this;
    }

    ~pair() = default;

    void swap(pair &other) {
        if (this != other) {
            easystl::swap(first, other.first);
            easystl::swap(second, other.second);
        }
    }
};

template <class Ty1, class Ty2>
bool operator==(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <class Ty1, class Ty2>
bool operator!=(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
    return !(lhs == rhs);
}

template <class Ty1, class Ty2>
bool operator<(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
    return lhs.first < rhs.first ||
           (lhs.first == rhs.first && lhs.second < rhs.second);
}

template <class Ty1, class Ty2>
bool operator>(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
    return rhs < lhs;
}

template <class Ty1, class Ty2>
bool operator<=(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
    return !(rhs < lhs);
}

template <class Ty1, class Ty2>
bool operator>=(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
    return !(lhs < rhs);
}

template <class Ty1, class Ty2>
void swap(pair<Ty1, Ty2> &lhs, pair<Ty1, Ty2> &rhs) {
    lhs.swap(rhs);
}

template <class Ty1, class Ty2>
pair<Ty1, Ty2> make_pair(Ty1 &&first, Ty2 &&second) {
    return pair<Ty1, Ty2>(easystl::forward<Ty1>(first),
                          easystl::forward<Ty2>(second));
}

} // namespace easystl

#endif // !EASYSTL_UTILITY_H
