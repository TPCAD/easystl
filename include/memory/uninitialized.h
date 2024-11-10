#ifndef EASYSTL_UNINITIALIZED_H
#define EASYSTL_UNINITIALIZED_H

#ifndef HEADER_STYLE
#include "algobase.h"
#include "construct.h"
#include "iterator/iterator_traits.h"
#else
#include "../algobase.h"
#include "../construct.h"
#include "../iterator/iterator_traits.h"

#endif // HEADER_STYLE

#include "alloc_traits.h"
#include <type_traits>

namespace easystl {

// 平凡类型的默认构造函数可能会被删除，而 std::copy 等只使用赋值而不是构造，
// 因此需要显示检查从 U 构造是否有效。否则不恰当的使用 uninitialized_xxx 可能
// 不会引起编译时错误。
template <typename T, typename U> constexpr bool __check_constructible() {
    static_assert(std::is_constructible<T, U>::value,
                  "result type must be constructible from input type");
    return true;
}

#define EASYSTL_USE_ASSIGN_FRO_INIT(T, U)                                      \
    std::is_trivial<T>::value &&std::is_assignable<T &, U>::value &&           \
    easystl::__check_constructible<T, U>()

/**
 *  @brief  uninitialized_copy 辅助函数
 *  @param  first  输入迭代器
 *  @param  last  输入迭代器
 *  @param  result  输出迭代器
 *  @return  result + (first - last)
 *
 *  处理不可平凡构造的元素
 */
template <bool TrivialValueType> struct __uninitialized_copy {
    template <typename InputIterator, typename ForwardIterator>
    ForwardIterator __uninit_copy(InputIterator first, InputIterator last,
                                  ForwardIterator result) {
        ForwardIterator cur = result;
        try {
            for (; first != last; ++first, (void)++cur) {
                std::_Construct(std::__addressof(*cur), *first);
                return cur;
            }
        } catch (...) {
            std::_Destroy(result, cur);
            throw;
        }
    }
};

/**
 *  @brief  uninitialized_copy 辅助函数
 *  @param  first  输入迭代器
 *  @param  last  输入迭代器
 *  @param  result  输出迭代器
 *  @return  result + (first - last)
 *
 *  处理可平凡构造的元素
 */
template <> struct __uninitialized_copy<true> {
    template <typename InputIterator, typename ForwardIterator>
    static ForwardIterator __uninit_copy(InputIterator first,
                                         InputIterator last,
                                         ForwardIterator result) {
        return easystl::copy(first, last, result);
    }
};

/**
 *  @brief  拷贝 [first, last) 的元素到 result
 *  @param  first  输入迭代器
 *  @param  last  输入迭代器
 *  @param  result  输出迭代器
 *  @return  result + (first - last)
 *
 *  将 [first, last) 范围内的元素拷贝到未初始化的区域。
 *  若元素是可平凡赋值则调用 copy()， 若非平凡赋值，则需要调用对应有参构造函数
 */
template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
                                   ForwardIterator result) {
    using input_type = typename iterator_traits<InputIterator>::value_type;
    using result_type = typename iterator_traits<ForwardIterator>::value_type;

    using From = decltype(*first);
    const bool assinable = EASYSTL_USE_ASSIGN_FRO_INIT(result_type, From);

    const bool can_memmove = std::is_trivial<input_type>::value;

    return easystl::__uninitialized_copy < can_memmove &&
           assinable > ::__uninit_copy(first, last, result);
}

/**
 *  @brief  拷贝 [first, last) 的元素到 result
 *  @param  first  输入迭代器
 *  @param  last  输入迭代器
 *  @param  result  输出迭代器
 *  @param  alloc  分配器
 *  @return  result + (first - last)
 *
 *  使用指定分配器进行复制
 */
template <typename InputIterator, typename ForwardIterator, typename Allocator>
ForwardIterator __uninitialized_copy_a(InputIterator first, InputIterator last,
                                       ForwardIterator result,
                                       Allocator &alloc) {
    ForwardIterator cur = result;
    try {
        using traits = easystl_cxx::alloc_traits<Allocator>;
        for (; first != last; ++first, (void)++cur) {
            traits::construct(alloc, std::__addressof(*cur), *first);
        }
        return cur;

    } catch (...) {
        std::_Destroy(result, cur, alloc);
        throw;
    }
}

/**
 *  @brief  拷贝 [first, last) 的元素到 result
 *  @param  first  输入迭代器
 *  @param  last  输入迭代器
 *  @param  result  输出迭代器
 *  @param  alloc  分配器
 *  @return  result + (first - last)
 *
 *  使用 easystl::allocator 进行复制
 */
template <typename InputIterator, typename ForwardIterator, typename Tp>
ForwardIterator __uninitialized_copy_a(InputIterator first, InputIterator last,
                                       ForwardIterator result,
                                       easystl::allocator<Tp> &) {
    return easystl::uninitialized_copy(first, last, result);
}

template <typename ForwardIterator, typename Allocator>
void uninitialized_default_a(ForwardIterator first, ForwardIterator last,
                             Allocator &alloc) {
    ForwardIterator cur = first;
    try {
        using traits = easystl_cxx::alloc_traits<Allocator>;
        for (; cur != last; ++cur) {
            traits::construct(alloc, std::__addressof(*cur));
        }
        return cur;
    } catch (...) {
        std::_Destroy(first, cur, alloc);
        throw;
    }
}

/**
 *  @brief  使用分配器在 [first, first+n) 上进行无参构造
 *  @param  first  起始地址
 *  @param  n  数量
 *  @param  alloc  分配器
 *  @return  结束地址
 */
template <typename ForwardIterator, typename Size, typename Allocator>
ForwardIterator uninitialized_default_n_a(ForwardIterator first, Size n,
                                          Allocator &alloc) {
    ForwardIterator cur = first;
    try {
        using traits = easystl_cxx::alloc_traits<Allocator>;
        for (; n > 0; --n, (void)++cur) {
            traits::construct(alloc, std::__addressof(*cur));
        }
        return cur;
    } catch (...) {
        std::_Destroy(first, cur, alloc);
        throw;
    }
}

/**
 *  @brief  使用分配器在 [first, first+n) 上进行拷贝构造
 *  @param  first  起始地址
 *  @param  n  数量
 *  @param  x  拷贝对象
 *  @param  alloc  分配器
 *  @return  结束地址
 */
template <typename ForwardIterator, typename Size, typename Tp,
          typename Allocator>
ForwardIterator uninitialized_fill_n_a(ForwardIterator first, Size n,
                                       const Tp &x, Allocator &alloc) {
    ForwardIterator cur = first;
    try {
        using traits = easystl_cxx::alloc_traits<Allocator>;
        for (; n > 0; --n, (void)++cur) {
            traits::construct(alloc, std::__addressof(*cur), x);
        }
        return cur;
    } catch (...) {
        std::_Destroy(first, cur, alloc);
        throw;
    }
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
