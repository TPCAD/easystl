#ifndef EASYSTL_ITERATOR_FUNCS_H
#define EASYSTL_ITERATOR_FUNCS_H

#include "iterator_traits.h"

namespace easystl {

// distance 的 input_iterator_tag 的版本
template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag) {
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
__distance(RandomIter first, RandomIter last, random_access_iterator_tag) {
    return last - first;
}

// 显式删除 output_iterator_tag 的版本
template <class OutputIterator>
void __distance(OutputIterator, OutputIterator, output_iterator_tag) = delete;

/**
 *  @brief  计算两个迭代器之间距离
 */
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    return __distance(first, last, iterator_category(first));
}

// 以下函数用于让迭代器前进 n 个距离

// advance 的 input_iterator_tag 的版本
template <class InputIterator, class Distance>
void __advance(InputIterator &i, Distance n, input_iterator_tag) {
    static_assert(n >= 0, "easystl::advance");
    while (n--)
        ++i;
}

// advance 的 bidirectional_iterator_tag 的版本
template <class BidirectionalIterator, class Distance>
void __advance(BidirectionalIterator &i, Distance n,
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
void __advance(RandomIter &i, Distance n, random_access_iterator_tag) {
    i += n;
}

/**
 *  @brief  使迭代器前进（或后退）
 */
template <class InputIterator, class Distance>
void advance(InputIterator &i, Distance n) {
    typename iterator_traits<InputIterator>::difference_type d = n;
    __advance(i, d, iterator_category(i));
}

/**
 *  @brief  使迭代器指向下一个元素
 */
template <typename InputIterator>
inline InputIterator
next(InputIterator x,
     typename iterator_traits<InputIterator>::difference_type n = 1) {
    easystl::advance(x, n);
    return x;
}

/**
 *  @brief  使迭代器指向上一个元素
 */
template <typename BidirectionIterator>
inline BidirectionIterator
prev(BidirectionIterator x,
     typename iterator_traits<BidirectionIterator>::difference_type n = 1) {
    easystl::advance(x, -n);
    return x;
}
} // namespace easystl

#endif // !EASYSTL_ITERATOR_FUNCS_H
