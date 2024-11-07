#ifndef EASYSTL_ITERATOR_FUNCS_H
#define EASYSTL_ITERATOR_FUNCS_H

#include "iterator_traits.h"

namespace easystl {

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
} // namespace easystl

#endif // !EASYSTL_ITERATOR_FUNCS_H
