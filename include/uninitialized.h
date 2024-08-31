#ifndef EASYSTL_UNINITIALIZED_H
#define EASYSTL_UNINITIALIZED_H

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include <type_traits>
namespace easystl {

/*
 * uninitialized_copy
 * 将 [first, last) 位置的内容拷贝到以 result
 * 为起始位置的空间中，返回复制结束的位置
 * */
template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_copy(InputIter first, InputIter last,
                                  ForwardIter result, std::true_type) {
    return copy(first, last, result);
}

template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_copy(InputIter first, InputIter last,
                                  ForwardIter result, std::false_type) {
    auto cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            construct(&*cur, *first);
        }
    } catch (...) {
        for (; result != cur; ++result) {
            destroy(&*result);
        }
    }
    return cur;
}

template <class InputIter, class ForwardIter>
ForwardIter uninitialized_copy(InputIter first, InputIter last,
                               ForwardIter result) {
    return unchecked_uninit_copy(
        first, last, result,
        std::is_trivially_copy_assignable<
            typename iterator_traits<ForwardIter>::value_type>{});
}

/*
 * uninitialized_copy
 * 将 [first, first + n) 位置的内容拷贝到以 result
 * 为起始位置的空间中，返回复制结束的位置
 * */
template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result,
                                    std::true_type) {
    return easystl::copy_n(first, n, result);
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result,
                                    std::false_type) {
    auto cur = result;
    try {
        for (; n > 0; --n, ++cur, ++first) {
            easystl::construct(&*cur, *first);
        }
    } catch (...) {
        for (; result != cur; --cur)
            easystl::destroy(&*cur);
    }
    return cur;
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result) {
    return easystl::unchecked_uninit_copy_n(
        first, n, result,
        std::is_trivially_copy_assignable<
            typename iterator_traits<InputIter>::value_type>{});
}

/*
 * uninitialized_fill_n
 * 从 first 位置开始填充，填充 n 个元素值，返回填充结束的位置
 * */

template <class ForwardIter, class Size, class T>
ForwardIter unchecked_uninit_fill_n(ForwardIter first, Size n, const T &value,
                                    std::true_type) {
    return fill_n(first, n, value);
}

template <class ForwardIter, class Size, class T>
ForwardIter unchecked_uninit_fill_n(ForwardIter first, Size n, const T &value,
                                    std::false_type) {
    auto cur = first;
    try {
        for (; n > 0; --n, ++cur) {
            construct(&*cur, value);
        }
    } catch (...) {
        for (; first != cur; ++first) {
            destroy(&*first);
        }
    }
    return cur;
}

template <class ForwardIter, class Size, class T>
ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T &value) {
    return unchecked_uninit_fill_n(
        first, n, value,
        std::is_trivially_copy_assignable<
            typename iterator_traits<ForwardIter>::value_type>{});
}

/*
 * uninitialized_move
 * 将 [first, last) 位置的内容移动到以 result
 * 为起始位置的空间中，返回移动结束的位置
 * */

template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_move(InputIter first, InputIter last,
                                  ForwardIter result, std::true_type) {
    return move(first, last, result);
}

template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_move(InputIter first, InputIter last,
                                  ForwardIter result, std::false_type) {
    ForwardIter cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            construct(&*cur, move(*first));
        }
    } catch (...) {
        destroy(result, cur);
    }
    return cur;
}

template <class InputIter, class ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last,
                               ForwardIter result) {
    return unchecked_uninit_move(
        first, last, result,
        std::is_trivially_move_assignable<
            typename iterator_traits<InputIter>::value_type>{});
}

} // namespace easystl

#endif // !EASYSTL_UNINITIALIZED_H
