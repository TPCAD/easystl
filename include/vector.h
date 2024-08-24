#ifndef EASYSTL_VECTOR_H
#define EASYSTL_VECTOR_H

#include "algo.h"
#include "allocator.h"
#include "exceptdef.h"
#include "iterator.h"
#include "memory.h"
#include "uninitialized.h"
#include "utility.h"
#include <initializer_list>
#include <type_traits>

namespace easystl {

template <class T> struct vector {

    static_assert(!std::is_same<bool, T>::value,
                  "vector<bool> is not supported now");

  public:
    typedef easystl::allocator<T> allocator_type;
    typedef easystl::allocator<T> data_allocator;

    typedef typename allocator_type::value_type value_type;
    typedef typename allocator_type::pointer pointer;
    typedef typename allocator_type::const_pointer const_pointer;
    typedef typename allocator_type::reference reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type size_type;
    typedef typename allocator_type::difference_type difference_type;

    typedef value_type *iterator;
    typedef const value_type *const_iterator;
    typedef easystl::reverse_iterator<iterator> reverse_iterator;
    typedef easystl::reverse_iterator<const_iterator> const_reverse_iterator;

    allocator_type get_allocator() { return data_allocator(); }

  private:
    iterator begin_;
    iterator end_;
    iterator cap_;

  public:
    // construcor
    vector() noexcept { try_init(); }

    explicit vector(size_type n) { fill_init(n, value_type()); }

    vector(size_type n, const value_type &value) { fill_init(n, value); }

    template <class Iter,
              typename std::enable_if<easystl::is_input_iterator<Iter>::value,
                                      int>::type = 0>
    vector(Iter first, Iter last) {
        EASYSTL_DEBUG(!(last < first));
        range_init(first, last);
    }

    // copy construcor
    vector(const vector &rhs) { range_init(rhs.begin_, rhs.end_); }

    // move construcor
    vector(vector &&rhs) noexcept
        : begin_(rhs.begin_), end_(rhs.end_), cap_(rhs.cap_) {
        rhs.begin_ = nullptr;
        rhs.end_ = nullptr;
        rhs.cap_ = nullptr;
    }
    // initializer_list construcor
    vector(std::initializer_list<value_type> ilist) {
        range_init(ilist.begin(), ilist.end());
    }

    // copy assignment
    vector &operator=(const vector &rhs);
    // move assignment
    vector &operator=(vector &&rhs) noexcept;
    // initializer_list assignment
    vector &operator=(std::initializer_list<value_type> ilist) {
        vector tmp(ilist.begin(), ilist.end());
        swap(tmp);
        return *this;
    }

    // deconstructor
    ~vector() {
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = end_ = cap_ = nullptr;
    }

  public:
    // iterator operation
    iterator begin() noexcept { return begin_; }
    const_iterator begin() const noexcept { return begin_; }
    iterator end() noexcept { return end_; }
    const_iterator end() const noexcept { return end_; }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator crend() const noexcept { return rend(); }

    // capacity operation
    bool empty() const noexcept { return begin_ == end_; }
    size_type size() const noexcept {
        return static_cast<size_type>(end_ - begin_);
    }
    size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(T);
    }
    size_type capacity() const noexcept {
        return static_cast<size_type>(cap_ - begin_);
    }
    void reserve(size_type n);
    void shrink_to_fit();

    // access elements operation
    reference operator[](size_type n) {
        EASYSTL_DEBUG(n < size());
        return *(begin_ + n);
    }
    const_reference operator[](size_type n) const {
        EASYSTL_DEBUG(n < size());
        return *(begin_ + n);
    }
    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(!(n < size()),
                              "vector<T>::at() subscript out of range");
        return (*this)[n];
    }
    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(!(n < size()),
                              "vector<T>::at() subscript out of range");
        return (*this)[n];
    }

    reference front() {
        EASYSTL_DEBUG(!empty());
        return *begin_;
    }
    const_reference front() const {
        EASYSTL_DEBUG(!empty());
        return *begin_;
    }
    reference back() {
        EASYSTL_DEBUG(!empty());
        return *(end_ - 1);
    }
    const_reference back() const {
        EASYSTL_DEBUG(!empty());
        return *(end_ - 1);
    }

    // data
    pointer data() noexcept { return begin_; }
    const_pointer data() const noexcept { return begin_; }

    // modifier
    // assign
    void assign(size_type n, const value_type &value) { fill_assign(n, value); }

    template <class Iter,
              typename std::enable_if<easystl::is_input_iterator<Iter>::value,
                                      int>::type = 0>
    void assign(Iter first, Iter last) {
        EASYSTL_DEBUG(!(last < first));
        copy_assign(first, last, iterator_category(first));
    }

    void assign(std::initializer_list<value_type> il) {
        copy_assign(il.begin(), il.end(), easystl::forward_iterator_tag{});
    }

    // emplace / emplace_back
    template <class... Args>
    iterator emplace(const_iterator pos, Args &&...args);

    template <class... Args> void emplace_back(Args &&...args);

    // push_back / pop_back
    void push_back(const value_type &value);
    void push_back(value_type &&value) { emplace_back(move(value)); }

    void pop_back();

    // insert
    iterator insert(const_iterator pos, const value_type &value);
    iterator insert(const_iterator pos, value_type &&value) {
        return emplace(pos, move(value));
    }

    iterator insert(const_iterator pos, size_type n, const value_type &value) {
        EASYSTL_DEBUG(pos >= begin() && pos <= end());
        return fill_insert(const_cast<iterator>(pos), n, value);
    }

    template <class Iter, typename std::enable_if<
                              is_input_iterator<Iter>::value, int>::type = 0>
    void insert(const_iterator pos, Iter first, Iter last) {
        EASYSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
        copy_insert(const_cast<iterator>(pos), first, last);
    }

    // erase/clear
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    void clear() { erase(begin(), end()); }

    // resize / reverse
    void resize(size_type new_size) { return resize(new_size, value_type()); }
    void resize(size_type new_size, const value_type &value);

    void reverse() { easystl::reverse(begin(), end()); }

    // swap
    void swap(vector &rhs) noexcept;

  private:
    /* helper func */
    // init and destroy
    void try_init() noexcept;
    void init_space(size_type size, size_type cap);
    void fill_init(size_type n, const value_type &value);
    template <class Iter> void range_init(Iter first, Iter last);
    void destroy_and_recover(iterator first, iterator last, size_type n);

    // calculate the growth size
    size_type get_new_cap(size_type add_size);

    // assign
    void fill_assign(size_type n, const value_type &value);

    template <class IIter>
    void copy_assign(IIter first, IIter last, input_iterator_tag);

    template <class FIter>
    void copy_assign(FIter first, FIter last, forward_iterator_tag);

    // reallocate
    template <class... Args>
    void reallocate_emplace(iterator pos, Args &&...args);
    void reallocate_insert(iterator pos, const value_type &value);

    // insert
    iterator fill_insert(iterator pos, size_type n, const value_type &value);
    template <class IIter>
    void copy_insert(iterator pos, IIter first, IIter last);

    // shrink_to_fit
    void reinsert(size_type size);
};

// copy assignment
template <class T> vector<T> &vector<T>::operator=(const vector &rhs) {
    if (this != *rhs) {
        const auto len = rhs.size();
        if (len > capacity()) {
            vector tmp(rhs.begin(), rhs.end());
            swap(tmp);
        } else if (size() >= len) {
            auto i = copy(rhs.begin(), rhs.end(), begin());
            data_allocator::destroy(i, end_);
            end_ = begin_ + len;
        } else {
            copy(rhs.begin(), rhs.begin() + size(), begin_);
            uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
            cap_ = end_ = begin_ + len;
        }
    }
    return *this;
}

// move assignment
template <class T> vector<T> &vector<T>::operator=(vector &&rhs) noexcept {
    destroy_and_recover(begin_, end_, cap_ - begin_);
    begin_ = rhs.begin_;
    end_ = rhs.end_;
    cap_ = rhs.cap_;
    rhs.begin_ = nullptr;
    rhs.end_ = nullptr;
    rhs.cap_ = nullptr;
    return *this;
}

// reserve() 预留 n 大小的空间，若不够则重新分配内存
template <class T> void vector<T>::reserve(size_type n) {
    if (capacity() < n) {
        THROW_LENGTH_ERROR_IF(
            n > max_size(),
            "n can not larger than max_size() in vector<T>::reserve(n)");

        const auto old_size = size();
        auto tmp = data_allocator::allocate(n);
        uninitialized_move(begin_, end_, tmp);
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = tmp;
        end_ = tmp + old_size;
        cap_ = begin_ + n;
    }
}

// shrink_to_fit() 放弃多余的容量
template <class T> void vector<T>::shrink_to_fit() {
    if (end_ < cap_) {
        reinsert(size());
    }
}

// emplace
template <class T>
template <class... Args>
typename vector<T>::iterator vector<T>::emplace(const_iterator pos,
                                                Args &&...args) {
    EASYSTL_DEBUG(pos >= begin() && pos <= end());
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = xpos - begin_;
    if (end_ != cap_ && xpos == end_) { // begin < pos = end < cap
        data_allocator::construct(address_of(*end_), forward<Args>(args)...);
        ++end_;
    } else if (end_ != cap_) { // begin < pos < end < cap
        auto new_end = end_;
        data_allocator::construct(address_of(*end_), *(end_ - 1));
        ++new_end;
        copy_backward(xpos, end_ - 1, end_);
        *xpos = value_type(forward<Args>(args)...);
        end_ = new_end;
    } else { // pos == begin
        reallocate_emplace(xpos, forward<Args>(args)...);
    }
    return begin() + n;
}

// emplace_back() 在尾部构造元素，避免额外的复制或移动开销
template <class T>
template <class... Args>
void vector<T>::emplace_back(Args &&...args) {
    if (end_ < cap_) {
        data_allocator::construct(address_of(*end_), forward<Args>(args)...);
        ++end_;
    } else {
        reallocate_emplace(end_, forward<Args>(args)...);
    }
}

// push_back() 在尾部插入元素
template <class T> void vector<T>::push_back(const value_type &value) {
    if (end_ != cap_) {
        data_allocator::construct(address_of((*end_)), value);
        ++end_;
    } else {
        reallocate_insert(end_, value);
    }
}

// pop_back() 弹出尾部元素
template <class T> void vector<T>::pop_back() {
    EASYSTL_DEBUG(!empty());
    data_allocator::destroy(end_ - 1);
    --end_;
}

// insert() 在 pos 迭代器处插入元素
template <class T>
typename vector<T>::iterator vector<T>::insert(const_iterator pos,
                                               const value_type &value) {
    EASYSTL_DEBUG(pos >= begin() && pos <= end());
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = pos - begin_;
    if (end_ != cap_ && xpos == end_) {
        data_allocator::construct(address_of(*end_), value);
        ++end_;
    } else if (end_ != cap_) {
        auto new_end = end_;
        data_allocator::construct(address_of(*end_), *(end_ - 1));
        ++new_end;
        auto value_copy = value;
        copy_backward(xpos, end_ - 1, end_);
        *xpos = move(value_copy);
        end_ = new_end;
    } else {
        reallocate_insert(xpos, value);
    }
    return begin_ + n;
}

// erase() 删除 pos 迭代器上的元素
template <class T>
typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
    EASYSTL_DEBUG(pos >= begin() && pos < end());
    iterator xpos = begin_ + (pos - begin());
    move(xpos + 1, end_, xpos);
    data_allocator::destroy(end_ - 1);
    --end_;
    return xpos;
}

// erase() 删除 [first, last) 迭代器范围上的元素
template <class T>
typename vector<T>::iterator vector<T>::erase(const_iterator first,
                                              const_iterator last) {
    EASYSTL_DEBUG(first >= begin() && last <= end() && !(last < first));
    const auto n = first - begin();
    iterator r = begin_ + (first - begin());
    data_allocator::destroy(move(r + (last - first), end_, r), end_);
    end_ = end_ - (last - first);
    return begin_ + n;
}

// resize() 重置 size
template <class T>
void vector<T>::resize(size_type new_size, const value_type &value) {
    if (new_size < size()) {
        erase(begin() + new_size, end());
    } else {
        insert(end(), new_size - size(), value);
    }
}

// swap() 与另一个 vector 交换
template <class T> void vector<T>::swap(vector<T> &rhs) noexcept {
    if (this != &rhs) {
        easystl::swap(begin_, rhs.begin_);
        easystl::swap(end_, rhs.end_);
        easystl::swap(cap_, rhs.cap_);
    }
}

// helper func

// try_init() 分配失败则忽略，不抛出异常
template <class T> void vector<T>::try_init() noexcept {
    try {
        begin_ = data_allocator::allocate(16);
        end_ = begin_;
        cap_ = begin_ + 16;
    } catch (...) {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
    }
}

// init_space() 分配 cap 大小的空间，失败则抛出异常
template <class T> void vector<T>::init_space(size_type size, size_type cap) {
    try {
        begin_ = data_allocator::allocate(cap);
        end_ = begin_ + size;
        cap_ = begin_ + cap;
    } catch (...) {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
        throw;
    }
}

// fill_init() 分配 n 大小的空间，并用 value 进行初始化
template <class T>
void vector<T>::fill_init(size_type n, const value_type &value) {
    const size_type init_size = max(static_cast<size_type>(16), n);
    init_space(n, init_size);
    uninitialized_fill_n(begin_, n, value);
}

// range_init() 分配 last - first 大小的空间，并拷贝 [first, last)
// 的内容到新空间
template <class T>
template <class Iter>
void vector<T>::range_init(Iter first, Iter last) {
    const size_type len = distance(first, last);
    const size_type init_size = max(len, static_cast<size_type>(16));
    init_space(len, init_size);
    uninitialized_copy(first, last, begin_);
}

// destroy_and_recover() 释放申请的内存
template <class T>
void vector<T>::destroy_and_recover(iterator first, iterator last,
                                    size_type n) {
    data_allocator::destroy(first, last);
    data_allocator::deallocate(first, n);
}

// get_new_cap() 容量将要满时确定扩容大小
template <class T>
typename vector<T>::size_type vector<T>::get_new_cap(size_type add_size) {
    const size_type old_size = capacity();
    THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size,
                          "vector<T> is too big");

    if (old_size > max_size() - old_size / 2) {
        return old_size + add_size > max_size() - 16 ? old_size + add_size
                                                     : old_size + add_size + 16;
    }
    const size_type new_size =
        old_size == 0 ? max(add_size, static_cast<size_type>(16))
                      : max(old_size + old_size / 2, old_size + add_size);
    return new_size;
}

// fill_assign() 使用 value 填充前 n 个元素
template <class T>
void vector<T>::fill_assign(size_type n, const value_type &value) {
    if (n > capacity()) { // n > capacity 重新分配空间并填充
        vector tmp(n, value);
        swap(tmp);
    } else if (n > size()) { // size < n < capacity
        fill(begin(), end(), value);
        end_ = uninitialized_fill_n(end_, n - size(), value);
    } else { // n < size 填充前 n 个元素，并清空后 size - n 个元素
        erase(fill_n(begin_, n, value), end_);
    }
}

// copy_assign
template <class T>
template <class IIter>
void vector<T>::copy_assign(IIter first, IIter last, input_iterator_tag) {
    auto cur = begin_;
    for (; first != last && cur != end_; ++first, ++cur) {
        *cur = *first;
    }
    if (first == last) {
        erase(cur, end_);
    } else {
        insert(end_, first, last);
    }
}

// copy_assign()
template <class T>
template <class FIter>
void vector<T>::copy_assign(FIter first, FIter last, forward_iterator_tag) {
    const size_type len = distance(first, last);
    if (len > capacity()) {
        vector tmp(first, last);
        swap(tmp);
    } else if (size() >= len) {
        auto new_end = copy(first, last, begin_);
        data_allocator::destroy(new_end, end_);
        end_ = new_end;
    } else {
        auto mid = first;
        advance(mid, size());
        copy(first, mid, begin_);
        auto new_end = uninitialized_copy(mid, last, end_);
        end_ = new_end;
    }
}

// reallocate_emplace() 重新分配内存，并在 pos 处就地构造对象
template <class T>
template <class... Args>
void vector<T>::reallocate_emplace(iterator pos, Args &&...args) {
    const auto new_size = get_new_cap(1);
    auto new_begin = data_allocator::allocate(new_size);
    auto new_end = new_begin;
    try {
        new_end = uninitialized_move(begin_, pos, new_begin);
        data_allocator::construct(address_of(*new_end), forward<Args>(args)...);
        ++new_end;
        new_end = uninitialized_move(pos, end_, new_end);
    } catch (...) {
        data_allocator::deallocate(new_begin, new_size);
        throw;
    }
    destroy_and_recover(begin_, end_, cap_ - begin_);
    begin_ = new_begin;
    end_ = new_end;
    cap_ = new_begin + new_size;
}

// reallocate_insert() 重新分配内存，并在 pos 处插入对象
template <class T>
void vector<T>::reallocate_insert(iterator pos, const value_type &value) {
    const auto new_size = get_new_cap(1);
    auto new_begin = data_allocator::allocate(new_size);
    auto new_end = new_begin;
    const value_type &value_copy = value;
    try {
        new_end = uninitialized_move(begin_, pos, new_begin);
        data_allocator::construct(address_of(*new_end), value_copy);
        ++new_end;
        new_end = uninitialized_move(pos, end_, new_end);
    } catch (...) {
        data_allocator::deallocate(new_begin, new_size);
        throw;
    }
    destroy_and_recover(begin_, end_, cap_ - begin_);
    begin_ = new_begin;
    end_ = new_end;
    cap_ = new_begin + new_size;
}

// fill_insert() 在 pos 迭代器处插入 n 个 value
template <class T>
typename vector<T>::iterator vector<T>::fill_insert(iterator pos, size_type n,
                                                    const value_type &value) {
    if (n == 0) {
        return pos;
    }

    const size_type xpos = pos - begin_;
    const value_type value_copy = value;

    if (static_cast<size_type>(cap_ - end_) >= n) { // 剩余空间足够
        const size_type after_elems = end_ - pos;
        auto old_end = end_;
        if (after_elems > n) {
            uninitialized_copy(end_ - n, end_, end_); // FIX: why copy here
            end_ += n;
            move_backward(pos, old_end - n, old_end);
            uninitialized_fill_n(pos, n, value_copy);
        } else {
            end_ = uninitialized_fill_n(end_, n - after_elems, value_copy);
            end_ = uninitialized_move(pos, old_end, end_);
            uninitialized_fill_n(pos, after_elems, value_copy);
        }
    } else {
        const auto new_size = get_new_cap(n);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try {
            new_end = uninitialized_move(begin_, pos, new_begin);
            new_end = uninitialized_fill_n(new_end, n, value);
            new_end = uninitialized_move(pos, end_, new_end);
        } catch (...) {
            destroy_and_recover(new_begin, new_end, new_size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = begin_ + new_size;
    }
    return begin_ + xpos;
}

// copy_insert()
template <class T>
template <class IIter>
void vector<T>::copy_insert(iterator pos, IIter first, IIter last) {
    if (last == first) {
        return;
    }

    const auto n = distance(first, last);

    if ((cap_ - end_) >= n) {
        const auto after_elems = end_ - pos;
        auto old_end = end_;
        if (after_elems > n) {
            end_ = uninitialized_copy(end_ - n, end_, end_);
            move_backward(pos, old_end - n, old_end);
            uninitialized_copy(first, last, pos);
        } else {
            auto mid = first;
            advance(mid, after_elems);
            end_ = uninitialized_copy(mid, last, end_);
            end_ = uninitialized_move(pos, old_end, end_);
            uninitialized_copy(first, mid, pos);
        }
    } else {
        const auto new_size = get_new_cap(n);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try {
            new_end = uninitialized_move(begin_, pos, new_begin);
            new_end = uninitialized_copy(first, last, new_end);
            new_end = uninitialized_move(pos, end_, new_end);
        } catch (...) {
            destroy_and_recover(new_begin, new_end, new_size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = begin_ + new_size;
    }
}

// reinsert
template <class T> void vector<T>::reinsert(size_type size) {
    auto new_begin = data_allocator::allocate(size);
    try {
        uninitialized_move(begin_, end_, new_begin);
    } catch (...) {
        data_allocator::deallocate(new_begin, size);
        throw;
    }
    data_allocator::deallocate(begin_, cap_ - begin_);
    begin_ = new_begin;
    end_ = new_begin + size;
    cap_ = begin_ + size;
}

// compare operator
template <class T> bool operator==(const vector<T> &lhs, const vector<T> &rhs) {
    return lhs.size() == rhs.size() &&
           equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T> bool operator<(const vector<T> &lhs, const vector<T> &rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                   rhs.end());
}

template <class T> bool operator!=(const vector<T> &lhs, const vector<T> &rhs) {
    return !(lhs == rhs);
}

template <class T> bool operator>(const vector<T> &lhs, const vector<T> &rhs) {
    return rhs < lhs;
}

template <class T> bool operator<=(const vector<T> &lhs, const vector<T> &rhs) {
    return !(rhs < lhs);
}

template <class T> bool operator>=(const vector<T> &lhs, const vector<T> &rhs) {
    return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class T> void swap(vector<T> &lhs, vector<T> &rhs) { lhs.swap(rhs); }

} // namespace easystl

#endif // !EASYSTL_VECTOR_H
