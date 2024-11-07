#ifndef EASYSTL_ITERATOR_TRAITS_H
#define EASYSTL_ITERATOR_TRAITS_H

#include <type_traits>

namespace easystl {

/**
 *  用于区分迭代器类型的空类型
 */
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <typename Iterator> struct iterator_traits {
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
};

// 针对原生指针的偏特化版本
template <typename T> struct iterator_traits<T *> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using difference_type = std::ptrdiff_t;
};

template <typename T> struct iterator_traits<const T *> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using pointer = const T *;
    using reference = const T &;
    using difference_type = std::ptrdiff_t;
};

// 萃取某个迭代器的 category
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator &) {
    return typename iterator_traits<Iterator>::iterator_category();
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

} // namespace easystl

#endif // !EASYSTL_ITERATOR_TRAITS_H
