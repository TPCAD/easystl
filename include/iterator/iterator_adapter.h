#ifndef EASYSTL_ITERATOR_ADAPTER_H
#define EASYSTL_ITERATOR_ADAPTER_H

#ifndef HEADER_STYLE
#include "ptr_traits.h"
#else
#include "../ptr_traits.h"
#endif // HEADER_STYLE

#include "iterator_traits.h"

namespace easystl {

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

template <typename Iterator>
inline reverse_iterator<Iterator> make_reverse_iterator(Iterator i) {
    return reverse_iterator<Iterator>(i);
}

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

#endif // !EASYSTL_ITERATOR_ADAPTER_H
