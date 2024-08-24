#ifndef EASYSTL_ALGO_H
#define EASYSTL_ALGO_H

#include "algobase.h"
#include "iterator.h"

namespace easystl {

/*
 * reverse
 * 将 [first, last) 区间内的元素反转
 * */
template <class BidirectionalIter>
void reverse_dispatch(BidirectionalIter first, BidirectionalIter last,
                      bidirectional_iterator_tag) {
    while (true) {
        if (first == last || first == --last) {
            return;
        }
        iter_swap(first++, last);
    }
}

template <class RandomIter>
void reverse_dispatch(RandomIter first, RandomIter last,
                      random_access_iterator_tag) {
    while (first < last) {
        iter_swap(first++, --last);
    }
}

template <class Iter> void reverse(Iter first, Iter last) {
    reverse_dispatch(first, last, iterator_category(first));
}
} // namespace easystl

#endif // !EASYSTL_ALGO_H
