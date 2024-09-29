#ifndef EASYSTL_BASIC_STRING_H
#define EASYSTL_BASIC_STRING_H

#include "algobase.h"
#include "allocator.h"
#include "char_traits.h"
#include "exceptdef.h"
#include "memory.h"
#include <cassert>
#include <cstddef>
#include <cstring>

namespace easystl {

#define STR_INIT_SIZE 15

template <class CharType, class CharTraits = easystl::char_traits<CharType>,
          class Allocator = easystl::allocator<CharType>>
struct basic_string {
  public:
    typedef CharTraits traits_type;
    typedef CharTraits char_traits;

    typedef Allocator allocator_type;
    typedef Allocator data_allocator;

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

    allocator_type get_allocator() { return allocator_type(); }

    static_assert(std::is_pod<CharType>::value,
                  "Character type of basic_string must be a POD");
    static_assert(
        std::is_same<CharType, typename traits_type::char_type>::value,
        "CharType must be same as traits_type::char_type");

  public:
    static constexpr size_type npos = static_cast<size_type>(-1);

  private:
    iterator buffer_; // 储存字符串的起始位置
    size_type size_;  // 大小
    size_type cap_;   // 容量

  public:
    // construct, copy, move, deconstruct

    /* construct an empty string
     */
    basic_string() noexcept { try_init(); }

    /* construct string with size and character
     *
     * @param n size
     * @param ch character to be filled
     */
    basic_string(size_type n, value_type ch)
        : buffer_(nullptr), size_(0), cap_(0) {
        fill_init(n, ch);
    }

    /* construct string with other string
     *
     * @param other other string
     * @param pos starting position of other string
     */
    basic_string(const basic_string &other, size_type pos)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(other.buffer_, pos, other.size_ - pos);
    }
    /* construct string with other string
     *
     * @param other other string
     * @param pos starting position of other string
     * @param count count to copy
     */
    basic_string(const basic_string &other, size_type pos, size_type count)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(other.buffer_, pos, count);
    }

    /* construct string with other c-style string
     *
     * @param str other string
     */
    basic_string(const_pointer str) : buffer_(nullptr), size_(0), cap_(0) {
        init_from(str, 0, char_traits::length(str));
    }
    /* construct string with other c-style string
     *
     * @param str other string
     * @param count count to copy
     */
    basic_string(const_pointer str, size_type count)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(str, 0, count);
    }

    /* construct string from a range
     *
     * @param first starting position of range
     * @param last end position of range
     */
    template <class Iter,
              typename std::enable_if<easystl::is_input_iterator<Iter>::value,
                                      int>::type = 0>
    basic_string(Iter first, Iter last) {
        copy_init(first, last, iterator_category(first));
    }

    /* copy constructor
     *
     * @param rhs other string
     */
    basic_string(const basic_string &rhs)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(rhs.buffer_, 0, rhs.size_);
    }

    /* move constructor
     *
     * @param rhs other string
     */
    basic_string(basic_string &&rhs) noexcept
        : buffer_(rhs.buffer_), size_(rhs.size_), cap_(rhs.cap_) {
        rhs.buffer_ = nullptr;
        rhs.size_ = 0;
        rhs.cap_ = 0;
    }

    /*
     * copy assignment
     * */
    basic_string &operator=(const basic_string &rhs);

    /*
     * move assignment
     * */
    basic_string &operator=(basic_string &&rhs) noexcept;

    /*
     * assignment with a c-style string
     *
     * @param str a c-style string
     * */
    basic_string &operator=(const_pointer str);

    /*
     * assignment with a character
     *
     * @param ch character
     * */
    basic_string &operator=(value_type ch);

    ~basic_string() { destroy_buffer(); }

  public:
    // iterator
    iterator begin() noexcept { return buffer_; }
    const_iterator begin() const noexcept { return buffer_; }
    iterator end() noexcept { return buffer_ + size_; }
    const_iterator end() const noexcept { return buffer_ + size_; }

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

    // capacity
    bool empty() const noexcept { return size_ == 0; }

    size_type size() const noexcept { return size_; }
    size_type length() const noexcept { return size_; }
    size_type capacity() const noexcept { return cap_; }
    size_type max_size() const noexcept { return static_cast<size_type>(-1); }

    /*
     * reserve space
     *
     * @param n size to reserved
     * */
    void reserve(size_type n);

    /* recover unused space
     */
    void shrink_to_fit();

    reference operator[](size_type n) {
        EASYSTL_DEBUG(n <= size_);
        if (n == size_)
            *(buffer_ + n) = value_type();
        return *(buffer_ + n);
    }
    const_reference operator[](size_type n) const {
        EASYSTL_DEBUG(n <= size_);
        if (n == size_)
            *(buffer_ + n) = value_type();
        return *(buffer_ + n);
    }

    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(n >= size_, "basic_string<Char, Traits>::at()"
                                          "subscript out of range");
        return (*this)[n];
    }
    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(n >= size_, "basic_string<Char, Traits>::at()"
                                          "subscript out of range");
        return (*this)[n];
    }

    reference front() {
        EASYSTL_DEBUG(!empty());
        return *begin();
    }
    const_reference front() const {
        EASYSTL_DEBUG(!empty());
        return *begin();
    }

    reference back() {
        EASYSTL_DEBUG(!empty());
        return *(end() - 1);
    }
    const_reference back() const {
        EASYSTL_DEBUG(!empty());
        return *(end() - 1);
    }

    const_pointer data() const noexcept { return to_raw_pointer(); }
    const_pointer c_str() const noexcept { return to_raw_pointer(); }

    // insert

    /* insert a character in specific position
     *
     * @param pos pointer of specific position
     * @param ch character to insert
     *
     * @return pointer of inserted position
     */
    iterator insert(const_iterator pos, value_type ch);

    /* insert a character multiply in specific position
     *
     * @param pos pointer of specific position
     * @param count amount to insert
     * @param ch character to insert
     *
     * @return pointer of inserted position
     */
    iterator insert(const_iterator pos, size_type count, value_type ch);

    /* insert a range of characters in specific position
     *
     * @param pos pointer of specific position
     * @param first starting pointer of range
     * @param last end pointer of range
     *
     * @return pointer of inserted position
     */
    template <class Iter>
    iterator insert(const_iterator pos, Iter first, Iter last);

    /* append a character at the end of string
     *
     * @param ch character to append
     */
    void push_back(value_type ch) { append(1, ch); }

    /* delete the last character of string
     */
    void pop_back() {
        EASYSTL_DEBUG(!empty());
        --size_;
    }

    // append

    /* append characters at the end of string
     *
     * @param count count to append
     * @param ch character to append
     *
     * @return reference of string itself
     */
    basic_string &append(size_type count, value_type ch);

    /* append a part of other string at the end of string
     *
     * @param str const reference of other string
     * @param pos starting position of other string
     * @param count count to append
     *
     * @return reference of string itself
     */
    basic_string &append(const basic_string &str, size_type pos,
                         size_type count);

    /* append a part of other c-style string at the end of string
     *
     * @param s other c-style string
     * @param count count to append
     *
     * @return reference of string itself
     */
    basic_string &append(const_pointer s, size_type count);

    /* append whole other string at the end of string
     *
     * @param str const reference of other string
     *
     * @return reference of string itself
     */
    basic_string &append(const basic_string &str) {
        return append(str, 0, str.size_);
    }

    /* append other string from specific position at the end of string
     *
     * @param str const reference of other string
     * @param pos starting position of other string
     *
     * @return reference of string itself
     */
    basic_string &append(const basic_string &str, size_type pos) {
        return append(str, pos, str.size_ - pos);
    }

    /* append whole other c-style string at the end of string
     *
     * @param s other c-style string
     *
     * @return reference of string itself
     */
    basic_string &append(const_pointer s) {
        return append(s, char_traits::length(s));
    }

    /* append characters from a range
     *
     * @param first starting position of range
     * @param last starting position of range
     *
     * @return the reference of string itself
     */
    template <class Iter,
              typename std::enable_if<easystl::is_input_iterator<Iter>::value,
                                      int>::type = 0>
    basic_string &append(Iter first, Iter last) {
        return append_range(first, last);
    }

    // erase /clear

    /* delete a character at specific position
     *
     * @param pos position of character
     *
     * @return pointer of deleted position
     */
    iterator erase(const_iterator pos);

    /* delete a range of characters
     *
     * @param first starting pointer of range
     * @param last end pointer of range
     *
     * @return pointer of starting deleted position
     */
    iterator erase(const_iterator first, const_iterator last);

    /* set size to 0
     */
    void clear() noexcept { size_ = 0; }

    // resize

    /* resize container
     *
     * @param count new size
     */
    void resize(size_type count) { resize(count, value_type()); }

    /* resize container
     *
     * @param count new size
     * @param ch character to fill if new size is larger
     */
    void resize(size_type count, value_type ch);

    // compare

    /* compare with other string
     *
     * @param other other string
     *
     * @return 0 for equal, -1 for less, 1 for great
     */
    int compare(const basic_string &other) const;

    /* part of string compares with other string
     *
     * @param pos starting position of string
     * @param count count to compare
     * @param other other string
     *
     * @return 0 for equal, -1 for less, 1 for great
     */
    int compare(size_type pos, size_type count,
                const basic_string &other) const;

    /* part of string compares with part of other string
     *
     * @param pos1 starting position of string
     * @param count1 count to compare
     * @param other other string
     * @param pos2 starting position of string
     * @param count2 count to compare
     *
     * @return 0 for equal, -1 for less, 1 for great
     */
    int compare(size_type pos1, size_type count1, const basic_string &other,
                size_type pos2, size_type count2 = npos) const;

    /* compare with c-style string
     *
     * @param s c-style string
     *
     * @return 0 for equal, -1 for less, 1 for great
     */
    int compare(const_pointer s) const;

    /* part of string compares with other c-style string
     *
     * @param pos starting position of string
     * @param count count to compare
     * @param s c-style string
     *
     * @return 0 for equal, -1 for less, 1 for great
     */
    int compare(size_type pos, size_type count, const_pointer s) const;

    /* part of string compares with part of other string
     *
     * @param pos1 starting position of string
     * @param count1 count to compare
     * @param s other c-style string
     * @param count2 count to compare
     *
     * @return 0 for equal, -1 for less, 1 for great
     */
    int compare(size_type pos1, size_type count1, const_pointer s,
                size_type count2) const;

    // substr

    /* substring
     *
     * @param index starting position of string
     * @param count count of substring
     *
     * @return a new string
     */
    basic_string substr(size_type index, size_type count = npos) {
        count = easystl::min(count, size_ - index);
        return basic_string(buffer_ + index, buffer_ + index + count);
    }

    // replace

    /* replace serveral characters starting from specific position with other
     * string
     *
     * @param pos starting position of string
     * @param count count to replace
     * @param str reference of other string
     *
     * @return reference of string itself
     */
    basic_string &replace(size_type pos, size_type count,
                          const basic_string &str) {
        THROW_OUT_OF_RANGE_IF(
            pos > size_,
            "basic_string<Char, Traits>::replace's pos out of range");
        return replace_cstr(buffer_ + pos, count, str.buffer_, str.size_);
    }

    /* replace a range of characters with other string
     *
     * @param first starting position of range
     * @param last end position of range
     * @param str reference of other string
     *
     * @return reference of string itself
     */
    basic_string &replace(const_iterator first, const_iterator last,
                          const basic_string &str) {
        EASYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
        return replace_cstr(first, static_cast<size_type>(last - first),
                            str.buffer_, str.size_);
    }

    /* replace serveral characters starting from specific position with other
     * c-style string
     *
     * @param pos starting position of string
     * @param count count to replace
     * @param str other c-style string
     *
     * @return reference of string itself
     */
    basic_string &replace(size_type pos, size_type count, const_pointer str) {
        THROW_OUT_OF_RANGE_IF(
            pos > size_,
            "basic_string<Char, Traits>::replace's pos out of range");
        return replace_cstr(buffer_ + pos, count, str,
                            char_traits::length(str));
    }

    /* replace a range of characters with other c-style string
     *
     * @param first starting position of range
     * @param last end position of range
     * @param str other c-style string
     *
     * @return reference of string itself
     */
    basic_string &replace(const_iterator first, const_iterator last,
                          const_pointer str) {
        EASYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
        return replace_cstr(first, static_cast<size_type>(last - first), str,
                            char_traits::length(str));
    }

    /* replace serveral characters starting from specific position with other
     * c-style string starting from specific position
     *
     * @param pos starting position of string
     * @param count1 count to replace
     * @param str other c-style string
     * @param count2 count to replace
     *
     * @return reference of string itself
     */
    basic_string &replace(size_type pos, size_type count1, const_pointer str,
                          size_type count2) {
        THROW_OUT_OF_RANGE_IF(
            pos > size_,
            "basic_string<Char, Traits>::replace's pos out of range");
        return replace_cstr(buffer_ + pos, count1, str, count2);
    }

    /* replace a range of characters with serveral characters from other c-style
     * string
     *
     * @param first starting position of range
     * @param last end position of range
     * @param str other c-style string
     * @param count count to replace
     *
     * @return reference of string itself
     */
    basic_string &replace(const_iterator first, const_iterator last,
                          const_pointer str, size_type count) {
        EASYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
        return replace_cstr(first, static_cast<size_type>(last - first), str,
                            count);
    }

    /* replace serveral characters starting from specific position with serveral
     * identical characters
     *
     * @param pos starting position of string
     * @param count1 count to replace
     * @param count2 count of filled characters
     * @param ch characters to fill
     *
     * @return reference of string itself
     */
    basic_string &replace(size_type pos, size_type count1, size_type count2,
                          value_type ch) {
        THROW_OUT_OF_RANGE_IF(
            pos > size_,
            "basic_string<Char, Traits>::replace's pos out of range");
        return replace_fill(buffer_ + pos, count1, count2, ch);
    }

    /* replace a range of characters with serveral identical characters
     *
     * @param first starting position of range
     * @param last end position of range
     * @param count count of filled characters
     * @param ch characters to fill
     *
     * @return reference of string itself
     */
    basic_string &replace(const_iterator first, const_iterator last,
                          size_type count, value_type ch) {
        EASYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
        return replace_fill(first, static_cast<size_type>(last - first), count,
                            ch);
    }

    /* replace serveral characters starting from specific position with serveral
     * characters starting from specific position of other string
     *
     * @param pos1 starting position of string
     * @param count1 count to replace
     * @param str reference of other string
     * @param pos2 starting position of other string
     * @param count2 count to replace
     *
     * @return reference of string itself
     */
    basic_string &replace(size_type pos1, size_type count1,
                          const basic_string &str, size_type pos2,
                          size_type count2 = npos) {
        THROW_OUT_OF_RANGE_IF(
            pos1 > size_ || pos2 > str.size_,
            "basic_string<Char, Traits>::replace's pos out of range");
        if (count2 == npos) {
            count2 = str.length() - pos2;
        }
        return replace_cstr(buffer_ + pos1, count1, str.buffer_ + pos2, count2);
    }

    /* replace a range of characters  with a range of characters
     *
     * @param first1 starting position of string
     * @param last1 end position of other string
     * @param first2 starting position of string
     * @param last2 end position of other string
     *
     * @return reference of string itself
     */
    template <class Iter,
              typename std::enable_if<easystl::is_input_iterator<Iter>::value,
                                      int>::type = 0>
    basic_string &replace(const_iterator first1, const_iterator last1,
                          Iter first2, Iter last2) {
        EASYSTL_DEBUG(begin() <= first1 && last1 <= end() && first1 <= last1);
        return replace_copy(first1, last1, first2, last2);
    }

    // reverse

    /* reverse characters
     */
    void reverse() noexcept;

    // swap

    /* swap two strings
     *
     * @param rhs other string
     */
    void swap(basic_string &rhs) noexcept;

    // find

    /* find the first occurrence of the character
     *
     * @param ch character to find
     * @param pos starting search position of string
     *
     * @return index or npos
     */
    size_type find(value_type ch, size_type pos = 0) const noexcept;

    /* find the first occurrence of the c-style string
     *
     * @param str c-style string to find
     * @param pos starting search position of string
     *
     * @return index or npos
     */
    size_type find(const_pointer str, size_type pos = 0) const noexcept {
        return find_cstr(str, pos, char_traits::length(str));
    };

    /* find the first occurrence of the first n characters of a c-style string
     *
     * @param str string to find
     * @param pos starting search position of string
     * @param count characters amount of c-style string
     *
     * @return index or npos
     */
    size_type find(const_pointer str, size_type pos,
                   size_type count) const noexcept {
        return find_cstr(str, pos, count);
    };

    /* find the first occurrence of the string
     *
     * @param str string to find
     * @param pos starting search position of string
     *
     * @return index or npos
     */
    size_type find(const basic_string &str, size_type pos = 0) const noexcept {
        return find_cstr(str.c_str(), pos, str.length());
    };

    // rfind

    /* find the last occurrence of the character
     *
     * @param ch character to find
     * @param pos starting search position of string
     *
     * @return index or npos
     */
    size_type rfind(value_type ch, size_type pos = npos) const noexcept;

    /* find the last occurrence of the c-style string
     *
     * @param str c-style string to find
     * @param pos starting search position of string
     *
     * @return index or npos
     */
    size_type rfind(const_pointer str, size_type pos = npos) const noexcept {
        return rfind_cstr(str, pos, char_traits::length(str));
    }

    /* find the last occurrence of the first n characters of a c-style string
     *
     * @param str string to find
     * @param pos starting search position of string
     * @param count characters amount of c-style string
     *
     * @return index or npos
     */
    size_type rfind(const_pointer str, size_type pos,
                    size_type count) const noexcept {
        return rfind_cstr(str, pos, count);
    }

    /* find the last occurrence of the string
     *
     * @param str string to find
     * @param pos starting search position of string
     *
     * @return index or npos
     */
    size_type rfind(const basic_string &str,
                    size_type pos = npos) const noexcept {
        return rfind_cstr(str.c_str(), pos, str.length());
    }

    // find_first_of
    // check if the string contains any character of the given character
    // sequence. return the first one's position or npos
    // FIX: check pos if great than size_
    size_type find_first_of(value_type ch, size_type pos = 0) const noexcept {
        return find(ch, pos);
    };

    size_type find_first_of(const_pointer s, size_type pos,
                            size_type count) const noexcept;
    size_type find_first_of(const_pointer s, size_type pos = 0) const noexcept {
        return find_first_of(s, pos, char_traits::length(s));
    };
    size_type find_first_of(const basic_string &str,
                            size_type pos = 0) const noexcept {
        return find_first_of(str.c_str(), pos, str.length());
    };

    // find_first_not_of
    // find the first character which is not in the given character sequence.
    // return its value or npos
    // FIX: check pos if great than size_
    size_type find_first_not_of(value_type ch,
                                size_type pos = 0) const noexcept;
    size_type find_first_not_of(const_pointer s, size_type pos,
                                size_type count) const noexcept;
    size_type find_first_not_of(const_pointer s,
                                size_type pos = 0) const noexcept {
        return find_first_not_of(s, pos, char_traits::length(s));
    };
    size_type find_first_not_of(const basic_string &str,
                                size_type pos = 0) const noexcept {
        return find_first_not_of(str, pos, str.length());
    };

    // find_last_of
    // check if the string contains any character of the given character
    // sequence. return the last one's position or npos
    // FIX: check pos if great than size_
    size_type find_last_of(value_type ch, size_type pos = 0) const noexcept;
    size_type find_last_of(const_pointer s, size_type pos,
                           size_type count) const noexcept;
    size_type find_last_of(const_pointer s, size_type pos = 0) const noexcept {
        return find_last_of(s, pos, char_traits::length(s));
    }
    size_type find_last_of(const basic_string &str,
                           size_type pos = 0) const noexcept {
        return find_last_of(str.c_str(), pos, str.length());
    }

    // find_last_not_of
    // FIX: check pos if great than size_
    size_type find_last_not_of(value_type ch, size_type pos = 0) const noexcept;
    size_type find_last_not_of(const_pointer s, size_type pos,
                               size_type count) const noexcept;
    size_type find_last_not_of(const_pointer s,
                               size_type pos = 0) const noexcept {
        return find_last_not_of(s, pos, char_traits::length(s));
    };
    size_type find_last_not_of(const basic_string &str,
                               size_type pos = 0) const noexcept {
        return find_last_not_of(str.c_str(), pos, str.length());
    };

    // count
    size_type count(value_type ch, size_type pos = 0) const noexcept;

  public:
    // override operator+=
    basic_string &operator+=(const basic_string &str) { return append(str); }
    basic_string &operator+=(value_type ch) { return append(1, ch); }
    basic_string &operator+=(const_pointer str) {
        return append(str, str + char_traits::length(str));
    }

    // override operator >> / operatror <<

    friend std::istream &operator>>(std::istream &is, basic_string &str) {
        value_type *buf = new value_type[4096];
        is >> buf;
        basic_string tmp(buf);
        str = std::move(tmp);
        delete[] buf;
        return is;
    }

    friend std::ostream &operator<<(std::ostream &os, const basic_string &str) {
        for (size_type i = 0; i < str.size_; ++i)
            os << *(str.buffer_ + i);
        return os;
    }

    // helper func
  private:
    // init and destroy

    /* init with default size and no exception is thrown if failed
     */
    void try_init() noexcept;

    /* init basic_string and fill character
     *
     * @param n size of buffer
     * @param ch character to be filled
     */
    void fill_init(size_type n, value_type ch);

    /* init string from a range
     *
     * @param first starting position of range
     * @param last end position of range
     * @param nameless check if it is input_iterator_tag
     */
    template <class Iter>
    void copy_init(Iter first, Iter last, easystl::input_iterator_tag);

    /* init string from a range
     *
     * @param first starting position of range
     * @param last end position of range
     * @param nameless check if it is forward_iterator_tag
     */
    template <class Iter>
    void copy_init(Iter first, Iter last, easystl::forward_iterator_tag);

    /* init from other string
     *
     * @param src source string
     * @param pos starting position
     * @param count length
     */
    void init_from(const_pointer src, size_type pos, size_type n);

    /* deallocate buffer of string
     */
    void destroy_buffer();

    /* get raw pointer
     *
     * @return const pointer of starting position of string
     */
    const_pointer to_raw_pointer() const;

    /* allocate new buffer with new size and move char to new buffer
     *
     * @param size size of new buffer
     */
    void reinsert(size_type size);

    /* append characters from a range
     *
     * @param first starting position of range
     * @param last starting position of range
     *
     * @return the reference of string itself
     */
    template <class Iter> basic_string &append_range(Iter first, Iter last);

    /* compare  c-style string
     *
     * @param str1 string to be compared
     * @param n1 length of str1
     * @param str2 string to be compared
     * @param n2 length of str2
     *
     * @return 0 for equal, -1 for less, 1 for great
     */
    int compare_cstr(const_pointer str1, size_type n1, const_pointer str2,
                     size_type n2) const;

    /* find the first occurrence of a substring
     *
     * @param str substring
     * @param pos starting position
     * @param count count to substring
     *
     * @return index of starting position or npos
     */
    size_type find_cstr(const_pointer str, size_type pos,
                        size_type count) const noexcept;

    /* find the last occurrence of a substring
     *
     * @param str substring
     * @param pos starting position
     * @param count count to substring
     *
     * @return index of starting position or npos
     */
    size_type rfind_cstr(const_pointer str, size_type pos,
                         size_type count) const noexcept;

    // replace

    /* replace serveral characters starting from specific position with serveral
     * characters of other c-style string starting from specific position
     *
     * @param first starting position of string
     * @param count1 count to replace
     * @param str starting position of other string
     * @param count2 count to replace
     *
     * @return reference of string itself
     */
    basic_string &replace_cstr(const_iterator first, size_type count1,
                               const_pointer str, size_type count2);

    /* replace serveral characters starting from specific position with serveral
     * identical characters
     *
     * @param first starting position of string
     * @param count1 count to replace
     * @param count2 count of filled characters
     * @param ch characters to fill
     *
     * @return reference of string itself
     */
    basic_string &replace_fill(const_iterator first, size_type count1,
                               size_type count2, value_type ch);

    /* replace a range of characters  with a range of characters
     *
     * @param first1 starting position of string
     * @param last1 end position of other string
     * @param first2 starting position of string
     * @param last2 end position of other string
     *
     * @return reference of string itself
     */
    template <class Iter>
    basic_string &replace_copy(const_iterator first1, const_iterator last1,
                               Iter first2, Iter last2);

    /* reallocate memory
     *
     * @param need extra size
     */
    void reallocate(size_type need);

    /* reallocate memory and insert character in specific position
     *
     * @param pos position to insert character
     * @param n amount of character to be inserted
     * @param ch character to be inserted
     *
     * @return pointer to starting position of first inserted character
     */
    iterator reallocate_and_fill(iterator pos, size_type n, value_type ch);

    /* reallocate memory and insert a range of character in specific position
     *
     * @param pos position to insert character
     * @param first starting pointer of range
     * @param first end pointer of range
     *
     * @return pointer to starting position of first inserted character
     */
    iterator reallocate_and_copy(iterator pos, const_iterator first,
                                 const_iterator last);
};

template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::operator=(
    const basic_string &rhs) {
    if (this != &rhs) {
        basic_string tmp(rhs);
        swap(tmp);
    }
    return *this;
}

template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::operator=(
    basic_string &&rhs) noexcept {
    destroy_buffer();
    buffer_ = rhs.buffer_;
    size_ = rhs.size_;
    cap_ = rhs.cap_;
    rhs.buffer_ = nullptr;
    rhs.size_ = 0;
    rhs.cap_ = 0;
    return *this;
}

template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::operator=(const_pointer str) {
    const size_type len = char_traits::length(str);
    if (cap_ < len) {
        auto new_buffer = data_allocator::allocate(len + 1);
        data_allocator::deallocate(buffer_);
        buffer_ = new_buffer;
        cap_ = len + 1;
    }
    char_traits::copy(buffer_, str, len);
    size_ = len;
    return *this;
}

template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::operator=(value_type ch) {
    if (cap_ < 1) {
        auto new_buffer = data_allocator::allocate(2);
        data_allocator::deallocate(buffer_);
        buffer_ = new_buffer;
        cap_ = 2;
    }
    *buffer_ = ch;
    size_ = 1;
    return *this;
}

template <class CharType, class CharTraits, class Allocator>
void basic_string<CharType, CharTraits, Allocator>::reserve(size_type n) {
    if (cap_ < n) {
        THROW_LENGTH_ERROR_IF(n > max_size(),
                              "n can not larger than max_size()"
                              "in basic_string<Char,Traits>::reserve(n)");
        auto new_buffer = data_allocator::allocate(n);
        char_traits::move(new_buffer, buffer_, size_);
        buffer_ = new_buffer;
        cap_ = n;
    }
}

template <class CharType, class CharTraits, class Allocator>
void basic_string<CharType, CharTraits, Allocator>::shrink_to_fit() {
    if (size_ != cap_) {
        reinsert(size_);
    }
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::iterator
basic_string<CharType, CharTraits, Allocator>::insert(const_iterator pos,
                                                      value_type ch) {
    iterator r = const_cast<iterator>(pos);
    if (size_ == cap_) {
        return reallocate_and_fill(r, 1, ch);
    }
    char_traits::move(r + 1, r, end() - r);
    ++size_;
    *r = ch;
    return r;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::iterator
basic_string<CharType, CharTraits, Allocator>::insert(const_iterator pos,
                                                      size_type count,
                                                      value_type ch) {
    iterator r = const_cast<iterator>(pos);
    if (count == 0)
        return r;
    if (cap_ - size_ < count) {
        return reallocate_and_fill(r, count, ch);
    }
    if (pos == end()) {
        char_traits::fill(end(), ch, count);
        size_ += count;
        return r;
    }
    char_traits::move(r + count, r, end() - r);
    char_traits::fill(r, ch, count);
    size_ += count;
    return r;
}

template <class CharType, class CharTraits, class Allocator>
template <class Iter>
typename basic_string<CharType, CharTraits, Allocator>::iterator
basic_string<CharType, CharTraits, Allocator>::insert(const_iterator pos,
                                                      Iter first, Iter last) {
    iterator r = const_cast<iterator>(pos);
    const size_type len = easystl::distance(first, last);
    if (len == 0)
        return r;
    if (cap_ - size_ < len) {
        return reallocate_and_copy(r, first, last);
    }
    if (pos == end()) {
        easystl::uninitialized_copy(first, last, end());
        size_ += len;
        return r;
    }
    char_traits::move(r + len, r, len);
    easystl::uninitialized_copy(first, last, r);
    size_ += len;
    return r;
}

template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::append(size_type count,
                                                      value_type ch) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                          "basic_string<Char, Tratis>'s size too big");
    if (cap_ - size_ < count) {
        reallocate(count);
    }
    char_traits::fill(buffer_ + size_, ch, count);
    size_ += count;
    return *this;
}

template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::append(const basic_string &str,
                                                      size_type pos,
                                                      size_type count) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                          "basic_string<Char, Tratis>'s size too big");
    if (count == 0)
        return *this;
    if (cap_ - size_ < count) {
        reallocate(count);
    }
    char_traits::copy(buffer_ + size_, str.buffer_ + pos, count);
    size_ += count;
    return *this;
}

template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::append(const_pointer s,
                                                      size_type count) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                          "basic_string<Char, Tratis>'s size too big");
    if (cap_ - size_ < count) {
        reallocate(count);
    }
    char_traits::copy(buffer_ + size_, s, count);
    size_ += count;
    return *this;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::iterator
basic_string<CharType, CharTraits, Allocator>::erase(const_iterator pos) {
    EASYSTL_DEBUG(pos != end());
    iterator r = const_cast<iterator>(pos);
    char_traits::move(r, pos + 1, end() - pos - 1);
    --size_;
    return r;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::iterator
basic_string<CharType, CharTraits, Allocator>::erase(const_iterator first,
                                                     const_iterator last) {
    if (first == begin() && last == end()) {
        clear();
        return end();
    }
    const size_type n = end() - last;
    iterator r = const_cast<iterator>(first);
    char_traits::move(r, last, n);
    size_ -= (last - first);
    return r;
}

template <class CharType, class CharTraits, class Allocator>
void basic_string<CharType, CharTraits, Allocator>::resize(size_type count,
                                                           value_type ch) {
    if (count < size_) {
        erase(buffer_ + count, buffer_ + size_);
    } else {
        append(count - size_, ch);
    }
}

// compare

template <class CharType, class CharTraits, class Allocator>
int basic_string<CharType, CharTraits, Allocator>::compare(
    const basic_string &other) const {
    return compare_cstr(buffer_, size_, other.buffer_, other.size_);
}

template <class CharType, class CharTraits, class Allocator>
int basic_string<CharType, CharTraits, Allocator>::compare(
    size_type pos, size_type count, const basic_string &other) const {
    auto n1 = easystl::min(count, size_ - pos);
    return compare_cstr(buffer_ + pos, n1, other.buffer_, other.size_);
}

template <class CharType, class CharTraits, class Allocator>
int basic_string<CharType, CharTraits, Allocator>::compare(
    size_type pos1, size_type count1, const basic_string &other, size_type pos2,
    size_type count2) const {
    auto n1 = easystl::min(count1, size_ - pos1);
    auto n2 = easystl::min(count2, other.size_ - pos2);
    return compare_cstr(buffer_, n1, other.buffer_, n2);
}

template <class CharType, class CharTraits, class Allocator>
int basic_string<CharType, CharTraits, Allocator>::compare(
    const_pointer s) const {
    auto n2 = char_traits::length(s);
    return compare_cstr(buffer_, size_, s, n2);
}

template <class CharType, class CharTraits, class Allocator>
int basic_string<CharType, CharTraits, Allocator>::compare(
    size_type pos, size_type count, const_pointer s) const {
    auto n1 = easystl::min(count, size_ - pos);
    auto n2 = char_traits::length(s);
    return compare_cstr(buffer_, n1, s, n2);
}

template <class CharType, class CharTraits, class Allocator>
int basic_string<CharType, CharTraits, Allocator>::compare(
    size_type pos1, size_type count1, const_pointer s, size_type count2) const {
    auto n1 = easystl::min(count1, size_ - pos1);
    return compare_cstr(buffer_, n1, s, count2);
}

template <class CharType, class CharTraits, class Allocator>
void basic_string<CharType, CharTraits, Allocator>::reverse() noexcept {
    for (auto i = begin(), j = end(); i < j;) {
        easystl::iter_swap(i++, --j);
    }
}

template <class CharType, class CharTraits, class Allocator>
void basic_string<CharType, CharTraits, Allocator>::swap(
    basic_string &rhs) noexcept {
    if (this != &rhs) {
        easystl::swap(buffer_, rhs.buffer_);
        easystl::swap(size_, rhs.size_);
        easystl::swap(cap_, rhs.cap_);
    }
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::find(
    value_type ch, size_type pos) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        if (*(buffer_ + i) == ch)
            return i;
    }
    return npos;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::rfind(
    value_type ch, size_type pos) const noexcept {
    if (pos >= size_)
        pos = size_ - 1;
    for (auto i = pos; i != 0; --i) {
        if (*(buffer_ + i) == ch)
            return i;
    }
    return front() == ch ? 0 : npos;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::find_first_of(
    const_pointer s, size_type pos, size_type count) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < count; ++j) {
            if (ch == *(s + j))
                return i;
        }
    }
    return npos;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::find_first_not_of(
    value_type ch, size_type pos) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        if (*(buffer_ + i) != ch)
            return i;
    }
    return npos;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::find_first_not_of(
    const_pointer s, size_type pos, size_type count) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        value_type ch = *(buffer_ + i);
        size_type j = 0;
        for (; j < count; ++j) {
            if (ch == *(s + j))
                break;
        }
        if (j == count) {
            return i;
        }
    }
    return npos;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::find_last_of(
    value_type ch, size_type pos) const noexcept {

    for (auto i = size_ - 1; i >= pos; --i) {
        if (ch == *(buffer_ + i)) {
            return i;
        }
        if (i == 0) {
            break;
        }
    }
    return npos;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::find_last_of(
    const_pointer s, size_type pos, size_type count) const noexcept {
    for (auto i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < count; ++j) {
            if (ch == *(s + j))
                return i;
        }
        if (i == 0) {
            break;
        }
    }
    return npos;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::find_last_not_of(
    value_type ch, size_type pos) const noexcept {
    if (pos >= size_) {
        pos = size_ - 1;
    }
    for (auto i = size_ - 1; i >= pos; --i) {
        if (*(buffer_ + i) != ch)
            return i;
        if (i == 0) {
            break;
        }
    }
    return npos;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::find_last_not_of(
    const_pointer s, size_type pos, size_type count) const noexcept {
    if (pos >= size_) {
        pos = size_ - 1;
    }
    for (auto i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        size_type j = 0;
        for (; j < count; ++j) {
            if (*(s + j) == ch) {
                break;
            }
        }
        if (j == count) {
            return i;
        }
        if (i == 0) {
            break;
        }
    }
    return npos;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::count(
    value_type ch, size_type pos) const noexcept {
    size_type n = 0;
    for (auto i = pos; i < size_; ++i) {
        if (*(buffer_ + i) == ch)
            ++n;
    }
    return n;
}

// helper func

template <class CharType, class CharTraits, class Allocator>
void basic_string<CharType, CharTraits, Allocator>::try_init() noexcept {
    try {
        buffer_ =
            data_allocator::allocate(static_cast<size_type>(STR_INIT_SIZE));
        size_ = 0;
        cap_ = STR_INIT_SIZE;
    } catch (...) {
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
    }
}

template <class CharType, class CharTraits, class Allocator>
void basic_string<CharType, CharTraits, Allocator>::fill_init(size_type n,
                                                              value_type ch) {
    const auto init_size =
        easystl::max(static_cast<size_type>(STR_INIT_SIZE), n + 1);
    buffer_ = data_allocator::allocate(init_size);
    char_traits::fill(buffer_, ch, n);
    size_ = n;
    cap_ = init_size;
}

template <class CharType, class CharTraits, class Allocator>
template <class Iter>
void basic_string<CharType, CharTraits, Allocator>::copy_init(
    Iter first, Iter last, easystl::input_iterator_tag) {
    size_type n = easystl::distance(first, last);
    const auto init_size =
        easystl::max(static_cast<size_type>(STR_INIT_SIZE), n + 1);
    try {
        buffer_ = data_allocator::allocate(init_size);
        size_ = n;
        cap_ = init_size;
    } catch (...) {
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
        throw;
    }
    for (; n > 0; --n, ++first)
        append(*first);
}

template <class CharType, class CharTraits, class Allocator>
template <class Iter>
void basic_string<CharType, CharTraits, Allocator>::copy_init(
    Iter first, Iter last, easystl::forward_iterator_tag) {
    const size_type n = easystl::distance(first, last);
    const auto init_size =
        easystl::max(static_cast<size_type>(STR_INIT_SIZE), n + 1);
    try {
        buffer_ = data_allocator::allocate(init_size);
        size_ = n;
        cap_ = init_size;
        easystl::uninitialized_copy(first, last, buffer_);
    } catch (...) {
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
        throw;
    }
}

template <class CharType, class CharTraits, class Allocator>
void basic_string<CharType, CharTraits, Allocator>::init_from(const_pointer src,
                                                              size_type pos,
                                                              size_type count) {
    const auto init_size =
        easystl::max(static_cast<size_type>(STR_INIT_SIZE), count + 1);
    buffer_ = data_allocator::allocate(init_size);
    char_traits::copy(buffer_, src + pos, count);
    size_ = count;
    cap_ = init_size;
}

template <class CharType, class CharTraits, class Allocator>
void basic_string<CharType, CharTraits, Allocator>::destroy_buffer() {
    if (buffer_ != nullptr) {
        data_allocator::deallocate(buffer_, cap_);
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
    }
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::const_pointer
basic_string<CharType, CharTraits, Allocator>::to_raw_pointer() const {
    *(buffer_ + size_) = value_type();
    return buffer_;
}

template <class CharType, class CharTraits, class Allocator>
void basic_string<CharType, CharTraits, Allocator>::reinsert(size_type size) {
    auto new_buffer = data_allocator::allocate(size);
    try {
        char_traits::move(new_buffer, buffer_, size);
    } catch (...) {
        data_allocator::deallocate(new_buffer);
    }
    buffer_ = new_buffer;
    size_ = size;
    cap_ = size;
}

template <class CharType, class CharTraits, class Allocator>
template <class Iter>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::append_range(Iter first,
                                                            Iter last) {
    const size_type n = easystl::distance(first, last);
    THROW_LENGTH_ERROR_IF(size_ > max_size() - n,
                          "basic_string<Char, Tratis>'s size too big");
    if (cap_ - size_ < n) {
        reallocate(n);
    }
    easystl::uninitialized_copy_n(first, n, buffer_ + size_);
    size_ += n;
    return *this;
}

template <class CharType, class CharTraits, class Allocator>
int basic_string<CharType, CharTraits, Allocator>::compare_cstr(
    const_pointer str1, size_type n1, const_pointer str2, size_type n2) const {
    auto rlen = easystl::min(n1, n2);
    auto res = char_traits::compare(str1, str2, rlen);
    if (res != 0)
        return res;
    if (n1 < n2)
        return -1;
    if (n1 > n2)
        return 1;
    return 0;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::find_cstr(
    const_pointer str, size_type pos, size_type count) const noexcept {
    if (count == 0) {
        return pos;
    }
    if (size_ - pos < count) {
        return npos;
    }
    const auto left = size_ - count;
    for (auto i = pos; i <= left; ++i) {
        if (*(buffer_ + i) == *str) {
            size_type j = 1;
            for (; j < count; ++j) {
                if (*(buffer_ + i + j) != *(str + j)) {
                    break;
                }
            }
            if (j == count) {
                return i;
            }
        }
    }
    return npos;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::size_type
basic_string<CharType, CharTraits, Allocator>::rfind_cstr(
    const_pointer str, size_type pos, size_type count) const noexcept {
    if (count == 0) {
        return pos;
    }
    // pos starts from 0
    if (pos < count - 1) {
        return npos;
    }
    if (pos >= size_) {
        pos = size_ - 1;
    }
    for (auto i = pos; i >= count - 1; --i) {
        if (*(buffer_ + i) == *(str + count - 1)) {
            size_type j = 1;
            for (; j < count; ++j) {
                if (*(buffer_ + i - j) != *(str + count - j - 1)) {
                    break;
                }
            }
            if (j == count) {
                return i - count + 1;
            }
        }
    }
    return npos;
}

template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::replace_cstr(
    const_iterator first, size_type count1, const_pointer str,
    size_type count2) {
    if (static_cast<size_type>(cend() - first) < count1) {
        count1 = cend() - first;
    }
    if (count1 < count2) {
        const size_type add = count2 - count1;
        THROW_LENGTH_ERROR_IF(size_ > max_size() - add,
                              "basic_string<Char, Traits>'s size too big");
        if (size_ > cap_ - add) {
            reallocate(add);
        }
        pointer r = const_cast<pointer>(first);
        char_traits::move(r + count2, first + count1, end() - (first + count1));
        char_traits::copy(r, str, count2);
        size_ += add;
    } else {
        pointer r = const_cast<pointer>(first);
        char_traits::move(r + count2, first + count1, end() - (first + count1));
        char_traits::copy(r, str, count2);
        size_ -= (count1 - count2);
    }
    return *this;
}

template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::replace_fill(
    const_iterator first, size_type count1, size_type count2, value_type ch) {
    if (static_cast<size_type>(cend() - first) < count1) {
        count1 = cend() - first;
    }
    if (count1 < count2) {
        const size_type add = count2 - count1;
        THROW_LENGTH_ERROR_IF(size_ > max_size() - add,
                              "basic_string<Char, Traits>'s size too big");
        if (size_ > cap_ - add) {
            reallocate(add);
        }
        pointer r = const_cast<pointer>(first);
        char_traits::move(r + count2, first + count1, end() - (first + count1));
        char_traits::fill(r, ch, count2);
        size_ += add;
    } else {
        pointer r = const_cast<pointer>(first);
        char_traits::move(r + count2, first + count1, end() - (first + count1));
        char_traits::fill(r, ch, count2);
        size_ -= (count1 - count2);
    }
    return *this;
}

template <class CharType, class CharTraits, class Allocator>
template <class Iter>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::replace_copy(
    const_iterator first1, const_iterator last1, Iter first2, Iter last2) {
    size_type len1 = last1 - first1;
    size_type len2 = last2 - first2;
    if (len1 < len2) {
        const size_type add = len2 - len1;
        THROW_LENGTH_ERROR_IF(size_ > max_size() - add,
                              "basic_string<Char, Traits>'s size too big");
        if (size_ > cap_ - add) {
            reallocate(add);
        }
        pointer r = const_cast<pointer>(first1);
        char_traits::move(r + len2, first1 + len1, end() - (first1 + len1));
        char_traits::copy(r, first2, len2);
        size_ += add;
    } else {
        pointer r = const_cast<pointer>(first1);
        char_traits::move(r + len2, first1 + len1, end() - (first1 + len1));
        char_traits::copy(r, first2, len2);
        size_ -= (len1 - len2);
    }
    return *this;
}

template <class CharType, class CharTraits, class Allocator>
void basic_string<CharType, CharTraits, Allocator>::reallocate(size_type need) {
    const auto new_cap = easystl::max(cap_ + need, cap_ + (cap_ >> 1));
    auto new_buffer = data_allocator::allocate(new_cap);
    char_traits::move(new_buffer, buffer_, size_);
    data_allocator::deallocate(buffer_);
    buffer_ = new_buffer;
    cap_ = new_cap;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::iterator
basic_string<CharType, CharTraits, Allocator>::reallocate_and_fill(
    iterator pos, size_type n, value_type ch) {
    const auto r = pos - buffer_;
    const auto old_cap = cap_;
    const auto new_cap = easystl::max(old_cap + n, old_cap + (old_cap >> 1));
    auto new_buffer = data_allocator::allocate(new_cap);
    auto e1 = char_traits::move(new_buffer, buffer_, r) + r;
    auto e2 = char_traits::fill(e1, ch, n) + n;
    char_traits::move(e2, buffer_ + r, size_ - r);
    data_allocator::deallocate(buffer_, old_cap);
    buffer_ = new_buffer;
    size_ += n;
    cap_ = new_cap;
    return buffer_ + r;
}

template <class CharType, class CharTraits, class Allocator>
typename basic_string<CharType, CharTraits, Allocator>::iterator
basic_string<CharType, CharTraits, Allocator>::reallocate_and_copy(
    iterator pos, const_iterator first, const_iterator last) {
    const auto r = pos - buffer_;
    const auto old_cap = cap_;
    const size_type n = easystl::distance(first, last);
    const auto new_cap = easystl::max(old_cap + n, old_cap + (old_cap >> 1));
    auto new_buffer = data_allocator::allocate(new_cap);
    auto e1 = char_traits::move(new_buffer, buffer_, r) + r;
    auto e2 = easystl::uninitialized_copy_n(first, n, e1);
    char_traits::move(e2, buffer_ + r, size_ - r);
    data_allocator::deallocate(buffer_, old_cap);
    buffer_ = new_buffer;
    size_ += n;
    cap_ = new_cap;
    return buffer_ + r;
}

// override global operator

// override operator+
// string + string
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(const basic_string<CharType, CharTraits, Allocator> &lhs,
          const basic_string<CharType, CharTraits, Allocator> &rhs) {
    basic_string<CharType, CharTraits, Allocator> tmp(lhs);
    tmp.append(rhs);
    return tmp;
}

// c-style string + string
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(const CharType *lhs,
          const basic_string<CharType, CharTraits, Allocator> &rhs) {
    basic_string<CharType, CharTraits, Allocator> tmp(lhs);
    tmp.append(rhs);
    return tmp;
}

// character + string
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(CharType ch,
          const basic_string<CharType, CharTraits, Allocator> &rhs) {
    basic_string<CharType, CharTraits, Allocator> tmp(1, ch);
    tmp.append(rhs);
    return tmp;
}

// string + c-style string
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(const basic_string<CharType, CharTraits, Allocator> &lhs,
          const CharType *rhs) {
    basic_string<CharType, CharTraits, Allocator> tmp(lhs);
    tmp.append(rhs);
    return tmp;
}

// string + character
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(const basic_string<CharType, CharTraits, Allocator> &lhs,
          CharType ch) {
    basic_string<CharType, CharTraits, Allocator> tmp(lhs);
    tmp.append(1, ch);
    return tmp;
}

// string(move) + string
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(basic_string<CharType, CharTraits, Allocator> &&lhs,
          const basic_string<CharType, CharTraits, Allocator> &rhs) {
    basic_string<CharType, CharTraits, Allocator> tmp(easystl::move(lhs));
    tmp.append(rhs);
    return tmp;
}

// string + string(move)
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(const basic_string<CharType, CharTraits, Allocator> &lhs,
          basic_string<CharType, CharTraits, Allocator> &&rhs) {
    basic_string<CharType, CharTraits, Allocator> tmp(easystl::move(rhs));
    tmp.insert(tmp.begin(), lhs.begin(), lhs.end());
    return tmp;
}

// string(move) + string(move)
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(basic_string<CharType, CharTraits, Allocator> &&lhs,
          basic_string<CharType, CharTraits, Allocator> &&rhs) {
    basic_string<CharType, CharTraits, Allocator> tmp(easystl::move(lhs));
    tmp.append(rhs);
    return tmp;
}

// c-style string + string(move)
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(const CharType *lhs,
          basic_string<CharType, CharTraits, Allocator> &&rhs) {
    basic_string<CharType, CharTraits, Allocator> tmp(easystl::move(rhs));
    tmp.insert(
        tmp.begin(), lhs,
        lhs +
            basic_string<CharType, CharTraits, Allocator>::char_traits::length(
                lhs));
    return tmp;
}

// character + string(move)
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(CharType ch, basic_string<CharType, CharTraits, Allocator> &&rhs) {
    basic_string<CharType, CharTraits, Allocator> tmp(easystl::move(rhs));
    tmp.insert(tmp.begin(), ch);
    return tmp;
}

// string(move) + c-style string
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(basic_string<CharType, CharTraits, Allocator> &&lhs,
          const CharType *rhs) {
    basic_string<CharType, CharTraits, Allocator> tmp(easystl::move(lhs));
    tmp.append(rhs);
    return tmp;
}

// string(move) + character
template <class CharType, class CharTraits, class Allocator>
basic_string<CharType, CharTraits, Allocator>
operator+(basic_string<CharType, CharTraits, Allocator> &&lhs, CharType ch) {
    basic_string<CharType, CharTraits, Allocator> tmp(easystl::move(lhs));
    tmp.append(1, ch);
    return tmp;
}

template <class CharType, class CharTraits, class Allocator>
bool operator==(const basic_string<CharType, CharTraits, Allocator> &lhs,
                const basic_string<CharType, CharTraits, Allocator> &rhs) {
    return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
}

template <class CharType, class CharTraits, class Allocator>
bool operator!=(const basic_string<CharType, CharTraits, Allocator> &lhs,
                const basic_string<CharType, CharTraits, Allocator> &rhs) {
    return lhs.size() != rhs.size() || lhs.compare(rhs) != 0;
}

// override easystl::swap
template <class CharType, class CharTraits, class Allocator>
void swap(basic_string<CharType, CharTraits, Allocator> &lhs,
          basic_string<CharType, CharTraits, Allocator> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename CharT, typename Traits, typename Allocator>
constexpr typename easystl::basic_string<CharT, Traits, Allocator>::size_type
    easystl::basic_string<CharT, Traits, Allocator>::npos;

} // namespace easystl

#endif // !EASYSTL_BASIC_STRING_H
