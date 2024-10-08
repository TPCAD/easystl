#ifndef EASYSTL_ITERATOR_H
#define EASYSTL_ITERATOR_H

#include "alloc_traits.h"
#include "exceptdef.h"

namespace easystl {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// iterator template
template <class Category, class T, class Distance = ptrdiff_t,
          class Pointer = T *, class Reference = T &>
struct iterator {
    typedef Category iterator_category;
    typedef T value_type;
    typedef Pointer pointer;
    typedef Reference reference;
    typedef Distance difference_type;
};

template <typename Iterator, typename = std::__void_t<>>
struct iterator_traits_helper {};

template <typename Iterator>
struct iterator_traits_helper<
    Iterator,
    std::__void_t<typename Iterator::iterator_category,
                  typename Iterator::value_type,
                  typename Iterator::difference_type,
                  typename Iterator::pointer, typename Iterator::reference>> {
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type value_type;
    typedef typename Iterator::difference_type difference_type;
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::reference reference;
};

template <typename Iterator>
struct iterator_traits : public iterator_traits_helper<Iterator> {};
// // 检查是否拥有 iterator_category 成员
// template <class T> struct has_iterator_cat {
//   private:
//     struct two {
//         char a;
//         char b;
//     };
//     template <class U> static two test(...);
//     template <class U> static char test(typename U::iterator_category * =
//     0);
//
//   public:
//     static const bool value = sizeof(test<T>(0)) == sizeof(char);
// };
//
// template <class Iterator, bool> struct iterator_traits_impl {};
//
// template <class Iterator> struct iterator_traits_impl<Iterator, true> {
//     typedef typename Iterator::iterator_category iterator_category;
//     typedef typename Iterator::value_type value_type;
//     typedef typename Iterator::pointer pointer;
//     typedef typename Iterator::reference reference;
//     typedef typename Iterator::difference_type difference_type;
// };
//
// template <class Iterator, bool> struct iterator_traits_helper {};
//
// template <class Iterator>
// struct iterator_traits_helper<Iterator, true>
//     : public iterator_traits_impl<
//           Iterator,
//           std::is_convertible<typename Iterator::iterator_category,
//                               input_iterator_tag>::value ||
//               std::is_convertible<typename Iterator::iterator_category,
//                                   output_iterator_tag>::value> {};
//
// template <class Iterator>
// struct iterator_traits
//     : public iterator_traits_helper<Iterator,
//                                     has_iterator_cat<Iterator>::value> {};

// 针对原生指针的偏特化版本
template <class T> struct iterator_traits<T *> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef T *pointer;
    typedef T &reference;
    typedef ptrdiff_t difference_type;
};

template <class T> struct iterator_traits<const T *> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef const T *pointer;
    typedef const T &reference;
    typedef ptrdiff_t difference_type;
};

// 萃取某个迭代器的 category
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator &) {
    typedef typename iterator_traits<Iterator>::iterator_category Category;
    return Category();
}

template <typename Iter>
using iter_category_t = typename iterator_traits<Iter>::iterator_category;

template <typename InIter>
using RequireInputIter = typename std::enable_if<std::is_convertible<
    easystl::iter_category_t<InIter>, input_iterator_tag>::value>::type;

template <typename Iter, typename Cat = iter_category_t<Iter>>
struct is_random_access_iter
    : std::is_base_of<random_access_iterator_tag, Cat> {
    typedef std::is_base_of<random_access_iterator_tag, Cat> Base;
    enum { value = Base::value };
};

// template <class T, class U, bool =
// has_iterator_cat<iterator_traits<T>>::value> struct has_iterator_cat_of
//     : public m_bool_constant<std::is_convertible<
//           typename iterator_traits<T>::iterator_category, U>::value> {};

// 萃取某种迭代器
// template <class T, class U>
// struct has_iterator_cat_of<T, U, false> : public m_false_type {};
//
// template <class Iter>
// struct is_input_iterator
//     : public has_iterator_cat_of<Iter, input_iterator_tag> {};
//
// template <class Iter>
// struct is_output_iterator
//     : public has_iterator_cat_of<Iter, output_iterator_tag> {};
//
// template <class Iter>
// struct is_forward_iterator
//     : public has_iterator_cat_of<Iter, forward_iterator_tag> {};
//
// template <class Iter>
// struct is_bidirectional_iterator
//     : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};
//
// template <class Iter>
// struct is_random_access_iterator
//     : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

// template <class Iterator>
// struct is_iterator
//     : public m_bool_constant<is_input_iterator<Iterator>::value ||
//                              is_output_iterator<Iterator>::value> {};

// 萃取某个迭代器的 distance_type
template <class Iterator>
typename iterator_traits<Iterator>::difference_type *
distance_type(const Iterator &) {
    return static_cast<typename iterator_traits<Iterator>::difference_type *>(
        0);
}

// 萃取某个迭代器的 value_type
template <class Iterator>
typename iterator_traits<Iterator>::value_type *value_type(const Iterator &) {
    return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
}

// 以下函数用于计算迭代器间的距离

// distance 的 input_iterator_tag 的版本
template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

// distance 的 random_access_iterator_tag 的版本
template <class RandomIter>
typename iterator_traits<RandomIter>::difference_type
distance_dispatch(RandomIter first, RandomIter last,
                  random_access_iterator_tag) {
    return last - first;
}

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    return distance_dispatch(first, last, iterator_category(first));
}

// 以下函数用于让迭代器前进 n 个距离

// advance 的 input_iterator_tag 的版本
template <class InputIterator, class Distance>
void advance_dispatch(InputIterator &i, Distance n, input_iterator_tag) {
    EASYSTL_DEBUG(n >= 0);
    while (n--)
        ++i;
}

// advance 的 bidirectional_iterator_tag 的版本
template <class BidirectionalIterator, class Distance>
void advance_dispatch(BidirectionalIterator &i, Distance n,
                      bidirectional_iterator_tag) {
    if (n > 0)
        while (n--)
            ++i;
    else
        while (n++)
            --i;
}

// advance 的 random_access_iterator_tag 的版本
template <class RandomIter, class Distance>
void advance_dispatch(RandomIter &i, Distance n, random_access_iterator_tag) {
    i += n;
}

template <class InputIterator, class Distance>
void advance(InputIterator &i, Distance n) {
    typename iterator_traits<InputIterator>::difference_type d = n;
    advance_dispatch(i, d, iterator_category(i));
}

template <typename InputIterator>
inline InputIterator
next(InputIterator x,
     typename iterator_traits<InputIterator>::difference_type n = 1) {
    easystl::advance(x, n);
    return x;
}

template <typename BidirectionIterator>
inline BidirectionIterator
prev(BidirectionIterator x,
     typename iterator_traits<BidirectionIterator>::difference_type n = 1) {
    easystl::advance(x, -n);
    return x;
}

// 模板类 : reverse_iterator
// 代表反向迭代器，使前进为后退，后退为前进
template <class Iterator> class reverse_iterator {
  private:
    Iterator current; // 记录对应的正向迭代器
    typedef iterator_traits<Iterator> traits_type;

  public:
    typedef Iterator iterator_type;
    typedef typename iterator_traits<Iterator>::pointer pointer;
    typedef typename iterator_traits<Iterator>::difference_type difference_type;
    typedef typename iterator_traits<Iterator>::reference reference;
    // typedef
    //     typename iterator_traits<Iterator>::iterator_category
    //     iterator_category;
    // typedef typename iterator_traits<Iterator>::value_type value_type;
    //
    // typedef reverse_iterator<Iterator> self;

  public:
    // 构造函数
    reverse_iterator() noexcept(noexcept(Iterator())) : current() {}

    explicit reverse_iterator(iterator_type i) noexcept(noexcept(Iterator()))
        : current(i) {}

    reverse_iterator(const reverse_iterator &x) noexcept(
        noexcept(Iterator(x.current)))
        : current(x.current) {}

    reverse_iterator &operator=(const reverse_iterator &) = default;

    template <typename Iter>
    reverse_iterator(const reverse_iterator<Iter> &x) noexcept(
        noexcept(Iterator(x.current)))
        : current(x.current) {}

    template <typename Iter>
    reverse_iterator &operator=(const reverse_iterator<Iter> &x) noexcept(
        noexcept(current = x.current)) {
        current = x.current;
        return *this;
    }

  public:
    // 取出对应的正向迭代器
    iterator_type base() const noexcept(noexcept(Iterator(current))) {
        return current;
    }

    // 重载操作符
    reference operator*() const { // 实际对应正向迭代器的前一个位置
        auto tmp = current;
        return *--tmp;
    }
    pointer operator->() const {
        auto tmp = current;
        --tmp;
        return S_to_pointer(tmp);
    }

    // 前进(++)变为后退(--)
    reverse_iterator &operator++() {
        --current;
        return *this;
    }
    reverse_iterator operator++(int) {
        auto tmp = *this;
        --current;
        return tmp;
    }
    // 后退(--)变为前进(++)
    reverse_iterator &operator--() {
        ++current;
        return *this;
    }
    reverse_iterator operator--(int) {
        auto tmp = *this;
        ++current;
        return tmp;
    }

    reverse_iterator &operator+=(difference_type n) {
        current -= n;
        return *this;
    }
    reverse_iterator operator+(difference_type n) const {
        return reverse_iterator(current - n);
    }
    reverse_iterator &operator-=(difference_type n) {
        current += n;
        return *this;
    }
    reverse_iterator operator-(difference_type n) const {
        return reverse_iterator(current + n);
    }

    reference operator[](difference_type n) const { return *(*this + n); }

  private:
    template <typename Tp> static Tp *S_to_pointer(Tp *p) { return p; }

    template <typename Tp> static pointer S_to_pointer(Tp t) {
        return t.operator->();
    }
};

// 重载比较操作符
template <class Iterator>
bool operator==(const reverse_iterator<Iterator> &lhs,
                const reverse_iterator<Iterator> &rhs) {
    return lhs.base() == rhs.base();
}

template <class Iterator>
bool operator<(const reverse_iterator<Iterator> &lhs,
               const reverse_iterator<Iterator> &rhs) {
    return rhs.base() < lhs.base();
}

template <class Iterator>
bool operator!=(const reverse_iterator<Iterator> &lhs,
                const reverse_iterator<Iterator> &rhs) {
    return !(lhs == rhs);
}

template <class Iterator>
bool operator>(const reverse_iterator<Iterator> &lhs,
               const reverse_iterator<Iterator> &rhs) {
    return rhs < lhs;
}

template <class Iterator>
bool operator<=(const reverse_iterator<Iterator> &lhs,
                const reverse_iterator<Iterator> &rhs) {
    return !(rhs < lhs);
}

template <class Iterator>
bool operator>=(const reverse_iterator<Iterator> &lhs,
                const reverse_iterator<Iterator> &rhs) {
    return !(lhs < rhs);
}

template <typename IteratorL, typename IteratorR>
inline bool operator==(const reverse_iterator<IteratorL> &lhs,
                       const reverse_iterator<IteratorR> &rhs) {
    return lhs.base() == rhs.base();
}

template <typename IteratorL, typename IteratorR>
inline bool operator<(const reverse_iterator<IteratorL> &lhs,
                      const reverse_iterator<IteratorR> &rhs) {
    return lhs.base() > rhs.base();
}

template <typename IteratorL, typename IteratorR>
inline bool operator!=(const reverse_iterator<IteratorL> &lhs,
                       const reverse_iterator<IteratorR> &rhs) {
    return lhs.base() != rhs.base();
}

template <typename IteratorL, typename IteratorR>
inline bool operator>(const reverse_iterator<IteratorL> &lhs,
                      const reverse_iterator<IteratorR> &rhs) {
    return lhs.base() < rhs.base();
}

template <typename IteratorL, typename IteratorR>
inline bool operator<=(const reverse_iterator<IteratorL> &lhs,
                       const reverse_iterator<IteratorR> &rhs) {
    return lhs.base() >= rhs.base();
}

template <typename IteratorL, typename IteratorR>
inline bool operator>=(const reverse_iterator<IteratorL> &lhs,
                       const reverse_iterator<IteratorR> &rhs) {
    return lhs.base() <= rhs.base();
}

template <typename IteratorL, typename IteratorR>
inline auto operator-(const reverse_iterator<IteratorL> &lhs,
                      const reverse_iterator<IteratorR> &rhs)
    -> decltype(rhs.base() - lhs.base()) {
    return rhs.base() - lhs.base();
}

template <typename Iterator>
inline reverse_iterator<Iterator>
operator+(typename reverse_iterator<Iterator>::difference_type n,
          const reverse_iterator<Iterator> &x) {
    return reverse_iterator<Iterator>(x.base() - n);
}

// template <typename Iterator>
// inline reverse_iterator<Iterator> make_reverse_iterator(Iterator i) {
//     return reverse_iterator<Iterator>(i);
// }

} // namespace easystl
namespace easystl_cxx {

template <typename Iterator, typename Container> class normal_iterator {
  protected:
    Iterator M_current;

    typedef easystl::iterator_traits<Iterator> traits_type;

    template <typename Iter>
    using convertible_from = typename std::enable_if<
        std::is_convertible<Iter, Iterator>::value>::type;

  public:
    typedef Iterator iterator_type;
    typedef typename traits_type::iterator_category iterator_category;
    typedef typename traits_type::value_type value_type;
    typedef typename traits_type::difference_type difference_type;
    typedef typename traits_type::reference reference;
    typedef typename traits_type::pointer pointer;

    constexpr normal_iterator() noexcept : M_current(Iterator()) {}

    explicit normal_iterator(const Iterator &i) noexcept : M_current(i) {}

    // Allow iterator to const_iterator conversion
    template <typename Iter, typename = convertible_from<Iter>>
    normal_iterator(const normal_iterator<Iter, Container> &i) noexcept
        : M_current(i.base()) {}

    // Forward iterator requirements
    reference operator*() const noexcept { return *M_current; }

    pointer operator->() const noexcept { return M_current; }

    normal_iterator &operator++() noexcept {
        ++M_current;
        return *this;
    }

    normal_iterator operator++(int) noexcept {
        return normal_iterator(M_current++);
    }

    // Bidirectional iterator requirements
    normal_iterator &operator--() noexcept {
        --M_current;
        return *this;
    }

    normal_iterator operator--(int) noexcept {
        return normal_iterator(M_current--);
    }

    // Random access iterator requirements
    reference operator[](difference_type n) const noexcept {
        return M_current[n];
    }

    normal_iterator &operator+=(difference_type n) noexcept {
        M_current += n;
        return *this;
    }

    normal_iterator operator+(difference_type n) const noexcept {
        return normal_iterator(M_current + n);
    }

    normal_iterator &operator-=(difference_type n) noexcept {
        M_current -= n;
        return *this;
    }

    normal_iterator operator-(difference_type n) const _GLIBCXX_NOEXCEPT {
        return normal_iterator(M_current - n);
    }

    const Iterator &base() const noexcept { return M_current; }
};

// Forward iterator requirements
template <typename IteratorL, typename IteratorR, typename Container>
inline bool
operator==(const normal_iterator<IteratorL, Container> &lhs,
           const normal_iterator<IteratorR, Container> &rhs) noexcept {
    return lhs.base() == rhs.base();
}

template <typename Iterator, typename Container>
inline bool
operator==(const normal_iterator<Iterator, Container> &lhs,
           const normal_iterator<Iterator, Container> &rhs) noexcept {
    return lhs.base() == rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool
operator!=(const normal_iterator<IteratorL, Container> &lhs,
           const normal_iterator<IteratorR, Container> &rhs) noexcept {
    return lhs.base() != rhs.base();
}

template <typename Iterator, typename Container>
inline bool
operator!=(const normal_iterator<Iterator, Container> &lhs,
           const normal_iterator<Iterator, Container> &rhs) _GLIBCXX_NOEXCEPT {
    return lhs.base() != rhs.base();
}

// Random access iterator requirements
template <typename IteratorL, typename IteratorR, typename Container>
_GLIBCXX_NODISCARD inline bool
operator<(const normal_iterator<IteratorL, Container> &lhs,
          const normal_iterator<IteratorR, Container> &rhs) _GLIBCXX_NOEXCEPT {
    return lhs.base() < rhs.base();
}

template <typename Iterator, typename Container>
inline bool
operator<(const normal_iterator<Iterator, Container> &lhs,
          const normal_iterator<Iterator, Container> &rhs) noexcept {
    return lhs.base() < rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool
operator>(const normal_iterator<IteratorL, Container> &lhs,
          const normal_iterator<IteratorR, Container> &rhs) noexcept {
    return lhs.base() > rhs.base();
}

template <typename Iterator, typename Container>
inline bool
operator>(const normal_iterator<Iterator, Container> &lhs,
          const normal_iterator<Iterator, Container> &rhs) noexcept {
    return lhs.base() > rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool
operator<=(const normal_iterator<IteratorL, Container> &lhs,
           const normal_iterator<IteratorR, Container> &rhs) noexcept {
    return lhs.base() <= rhs.base();
}

template <typename Iterator, typename Container>
inline bool
operator<=(const normal_iterator<Iterator, Container> &lhs,
           const normal_iterator<Iterator, Container> &rhs) noexcept {
    return lhs.base() <= rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool
operator>=(const normal_iterator<IteratorL, Container> &lhs,
           const normal_iterator<IteratorR, Container> &rhs) noexcept {
    return lhs.base() >= rhs.base();
}

template <typename Iterator, typename Container>
inline bool
operator>=(const normal_iterator<Iterator, Container> &lhs,
           const normal_iterator<Iterator, Container> &rhs) noexcept {
    return lhs.base() >= rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline auto operator-(const normal_iterator<IteratorL, Container> &lhs,
                      const normal_iterator<IteratorR, Container> &rhs) noexcept
    -> decltype(lhs.base() - rhs.base()) {
    return lhs.base() - rhs.base();
}

template <typename Iterator, typename Container>
inline typename normal_iterator<Iterator, Container>::difference_type
operator-(const normal_iterator<Iterator, Container> &lhs,
          const normal_iterator<Iterator, Container> &rhs) noexcept {
    return lhs.base() - rhs.base();
}

template <typename Iterator, typename Container>
inline normal_iterator<Iterator, Container>
operator+(typename normal_iterator<Iterator, Container>::difference_type n,
          const normal_iterator<Iterator, Container> &i) noexcept {
    return normal_iterator<Iterator, Container>(i.base() + n);
}

template <typename Iterator, typename Container>
constexpr auto
to_address(const easystl_cxx::normal_iterator<Iterator, Container> &it) noexcept
    -> decltype(easystl::to_address(it.base())) {
    return easystl::to_address(it.base());
}

} // namespace easystl_cxx

#endif // !EASYSTL_ITERATOR_H
