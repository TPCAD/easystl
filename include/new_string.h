#ifndef EASYSTL_BASIC_STRING_H
#define EASYSTL_BASIC_STRING_H

#include "algobase.h"
#include "alloc_traits.h"
#include "allocator.h"
#include "char_traits.h"
#include "exceptdef.h"
#include "iterator.h"
#include "memory.h"
#include <cstring>
#include <functional>
#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace easystl {

template <class CharType, class CharTraits = easystl::char_traits<CharType>,
          class Allocator = easystl::allocator<CharType>>
struct basic_string {

  private:
    static_assert(std::is_same<CharType, typename CharTraits::char_type>::value,
                  "CharType must be same as CharTraits::char_type");
    static_assert(std::is_same<CharType, typename Allocator::value_type>::value,
                  "CharType must be same as Allocator::value_type");
    using char_alloc_type = Allocator;

    typedef easystl::alloc_traits<char_alloc_type> alloc_traits;

  public:
    typedef CharTraits traits_type;
    typedef typename CharTraits::char_type value_type;
    typedef char_alloc_type allocator_type;

    typedef typename alloc_traits::size_type size_type;
    typedef typename alloc_traits::difference_type difference_type;
    typedef typename alloc_traits::reference reference;
    typedef typename alloc_traits::const_reference const_reference;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::const_pointer const_pointer;

    // typedef value_type *iterator;
    // typedef const value_type *const_iterator;
    typedef easystl::normal_iterator<pointer, basic_string> iterator;
    typedef easystl::normal_iterator<const_pointer, basic_string>
        const_iterator;
    typedef easystl::reverse_iterator<iterator> reverse_iterator;
    typedef easystl::reverse_iterator<const_iterator> const_reverse_iterator;

    static constexpr size_type npos = static_cast<size_type>(-1);

    static_assert(std::is_pod<CharType>::value,
                  "Character type of basic_string must be a POD");

  private:
    /**
     *  @brief  使用分配器分配内存
     *  @param  a  使用的分配器
     *  @param  n  需要分配的内存的大小
     *  @return  指向新分配的内存的指针
     */
    static pointer S_allocate(char_alloc_type &a, size_type n) {
        pointer p = alloc_traits::allocate(a, n);
        return p;
    }

    struct alloc_hider : allocator_type {
        alloc_hider(pointer data, const Allocator &alloc)
            : allocator_type(alloc), M_ptr(data) {}

        alloc_hider(pointer data, Allocator &&alloc = Allocator())
            : allocator_type(easystl::move(alloc)), M_ptr(data) {}

        pointer M_ptr;
    };

    alloc_hider M_dataplus;
    size_type M_string_length;

    // 小字符串的容量，不包括空字符
    enum { S_local_capacity = 15 / sizeof(CharType) };

    // 小字符串优化，若字符长度小于小字符串容量，则使用该区域直接存储字符，若大
    // 于，则使用该区域存储字符串的容量大小
    union {
        CharType M_local_buf[S_local_capacity + 1];
        size_type M_allocated_capacity;
    };

    /**
     *  @brief  更新数据指针
     *  @param  ptr  新数据指针
     */
    void M_data(pointer ptr) { M_dataplus.M_ptr = ptr; }

    /**
     *  @brief  获取数据指针
     */
    pointer M_data() const { return M_dataplus.M_ptr; }

    /**
     *  @brief 更新字符串长度
     *
     *  @param len 新的长度
     */
    void M_length(size_type len) { M_string_length = len; }

    /**
     *  @brief 获取小字符串的数据指针
     */
    pointer M_local_data() {
        return pointer(M_local_buf);
        // return std::pointer_traits<const_pointer>::pointer_to(*local_buf);
    }

    /**
     *  @brief 获取常量形式的小字符串数据指针
     */
    const_pointer M_local_data() const {
        return const_pointer(M_local_buf);
        // return std::pointer_traits<const_pointer>::pointer_to(*local_buf);
    }

    /**
     *  @brief  更新容量
     *  @param  cap  新容量
     */
    void M_capacity(size_type cap) { M_allocated_capacity = cap; }

    /**
     *  @brief 更新字符串长度，同时在末尾添加空字符
     *
     *  @param len 新长度
     */
    void M_set_length(size_type n) {
        M_length(n);
        traits_type::assign(M_data()[n], CharType());
    }

    /**
     *  @brief  判断当前字符串是否为小字符串
     *  @return  bool
     */
    bool M_is_local() const { return M_data() == M_local_data(); }

    /**
     *  @brief  分配内存辅助函数
     *  @param  capacity  需要的容量
     *  @param  old_capacity  现有的容量
     *  @return  执行新内存的指针
     *  @throw  std::length_error  如果 @a capacity 大于最大长度
     *
     *  如果 @a capacity 大于 @a old_capacity，但小于两倍的 @a old_capacity，
     *  那么 @a capacity 将被置为两倍的 @a old_capacity，但不会大于最大长度。
     *  最终分配的 容量将是 @a capacity + 1（存放空字符）
     */
    pointer M_create(size_type &, size_type);

    /**
     *  @brief  释放已分配的内存
     */
    void M_dispose() {
        if (!M_is_local()) {
            M_destroy(M_allocated_capacity);
        }
    }

    /**
     *  @brief  释放已分配的内存
     */
    void M_destroy(size_type size) throw() {
        alloc_traits::deallocate(M_get_allocator(), M_data(), size + 1);
    }

    template <typename InputIter>
    void M_construct(InputIter first, InputIter end,
                     easystl::input_iterator_tag);

    template <typename ForwardIter>
    void M_construct(ForwardIter first, ForwardIter end,
                     easystl::forward_iterator_tag);

    void M_construct(size_type req, CharType c);

    allocator_type &M_get_allocator() { return M_dataplus; }

    const allocator_type &M_get_allocator() const { return M_dataplus; }

    // FIX: it seems no use if lower than c++20
    void M_init_local_buf() noexcept {}

    /**
     *  @brief  Get pointer of @a M_local_data.
     */
    pointer M_use_local_data() noexcept { return M_local_data(); }

  private:
    /**
     *  @brief  Check @a pos is not exceeds size().
     *  @param  pos  Index of first character to copy from.
     *  @param  s  Error message.
     *  @return  @a pos
     *
     *  @throw  Throw std::out_of_range if @a pos is larger than size()
     */
    size_type M_check(size_type pos, const char *s) const {
        THROW_OUT_OF_RANGE_IF(pos > this->size(), s);
        return pos;
    }

    void M_check_length(size_type n1, size_type n2, const char *s) {
        THROW_LENGTH_ERROR_IF(this->max_size() - (this->size() - n1) < n2, s);
    }

    /**
     *  @brief  确保 @a off 小于 size() - pos
     *  @param  pos  起始字符的索引
     *  @param  off  偏移量
     *  @return  @a off 和 size() - pos 二者的较小值
     */
    size_type M_limit(size_type pos, size_type off) const noexcept {
        const bool testoff = off < this->size() - pos;
        return testoff ? off : this->size() - pos;
    }

    // check pointer s and data do not overlap
    bool M_disjunct(const CharType *s) const noexcept {
        return (std::less<const CharType *>()(s, M_data()) ||
                std::less<const CharType *>()(M_data() + this->size(), s));
    }

    /**
     *  @brief  Copy characters from source to destination
     *  @param  d  pointer to destination
     *  @param  s  pointer to source
     *  @param  n  size of characters to copy
     */
    static void S_copy(CharType *d, const CharType *s, size_type n) {
        if (n == 1)
            traits_type::assign(*d, *s);
        else
            traits_type::copy(d, s, n);
    }

    static void S_move(CharType *d, const CharType *s, size_type n) {
        if (n == 1)
            traits_type::assign(*d, *s);
        else
            traits_type::move(d, s, n);
    }

    /**
     *  @brief  Assign a sequence of characters from source to destination.
     *  @param  d  Destination.
     *  @param  n  Number of characters.
     *  @param  s  Source.
     */
    static void S_assign(CharType *d, size_type n, CharType c) {
        if (n == 1)
            traits_type::assign(*d, c);
        else
            traits_type::assign(d, n, c);
    }

    /**
     *  @brief  Copy characters from a range to destination
     *  @param  p  pointer to destination
     *  @param  first  starting iterator
     *  @param  end  end iterator
     */
    template <typename Iter>
    static void S_copy_chars(CharType *p, Iter first, Iter end) {
        for (; first != end; (void)++p, ++first) {
            traits_type::assign(*p, *first);
        }
    }

    /**
     *  @brief  Copy characters from a range to destination
     *  @param  p  pointer to destination
     *  @param  first  starting iterator
     *  @param  end  end iterator
     */
    static void S_copy_chars(CharType *p, iterator first,
                             iterator end) noexcept {
        S_copy_chars(p, first, end);
    }

    /**
     *  @brief  Copy characters from a range to destination
     *  @param  p  pointer to destination
     *  @param  first  starting iterator
     *  @param  end  end iterator
     */
    static void S_copy_chars(CharType *p, const_iterator first,
                             const_iterator end) noexcept {
        S_copy_chars(p, first, end);
    }

    /**
     *  @brief  Copy characters from a range to destination
     *  @param  p  pointer to destination
     *  @param  first  starting pointer
     *  @param  end  end pointer
     */
    static void S_copy_chars(CharType *p, CharType *first,
                             CharType *end) noexcept {
        S_copy(p, first, end - first);
    }

    /**
     *  @brief  Copy characters from a range to destination
     *  @param  p  pointer to destination
     *  @param  first  const starting pointer
     *  @param  end  const end pointer
     */
    static void S_copy_chars(CharType *p, const CharType *first,
                             const CharType *end) noexcept {
        S_copy(p, first, end - first);
    }

    static int S_compare(size_type n1, size_type n2) noexcept {
        const difference_type d = difference_type(n1 - n2);
        if (d > std::numeric_limits<int>::max()) {
            return std::numeric_limits<int>::max();
        } else if (d < std::numeric_limits<int>::min()) {
            return std::numeric_limits<int>::min();
        } else {
            return int(d);
        }
    }

    void M_assign(const basic_string &);

    // TODO: comment
    void M_mutate(size_type pos, size_type len1, const CharType *s,
                  size_type len2);

    // TODO: comment
    void M_erase(size_type pos, size_type n);

  public:
    /**
     *  @brief  Default constructor creates an empty string.
     */
    basic_string() noexcept(
        std::is_nothrow_default_constructible<Allocator>::value)
        : M_dataplus(M_local_data()) {
        M_init_local_buf();
        M_set_length(0);
    }

    /**
     *  @brief  Construct an empty string using allocator @a a.
     */
    basic_string(const Allocator &a) noexcept : M_dataplus(M_local_data(), a) {
        M_init_local_buf();
        M_set_length(0);
    }

    /**
     *  @brief  Construct string with copy of value of @a str.
     *  @param  str  Source string.
     */
    basic_string(const basic_string &str) : M_dataplus(M_local_data()) {
        M_construct(str.M_data(), str.M_data() + str.length(),
                    easystl::forward_iterator_tag());
    }

    /**
     *  @brief  Construct string as copy of a substring.
     *  @param  str  Source string.
     *  @param  pos  Index of first character to copy from.
     *  @param  a  Allocator to use.
     */
    basic_string(const basic_string &str, size_type pos,
                 const Allocator &a = Allocator())
        : M_dataplus(M_local_data(), a) {
        const CharType *start =
            str.M_data() +
            str.M_check(pos, "basic_string::basic_string out of range");
        M_construct(start, start + str.M_limit(pos, npos),
                    easystl::forward_iterator_tag());
    }

    /**
     *  @brief  Construct string as copy of a substring.
     *  @param  str  Source string.
     *  @param  pos  Index of first character to copy from.
     *  @param  n  Number of characters to copy.
     */
    basic_string(const basic_string &str, size_type pos, size_type n)
        : M_dataplus(M_local_data()) {
        const CharType *start =
            str.M_data() +
            str.M_check(pos, "basic_string::basic_string out of string");
        M_construct(start, start + str.M_limit(pos, n),
                    easystl::forward_iterator_tag());
    }

    /**
     *  @brief  Construct string as copy of a substring.
     *  @param  str  Source string.
     *  @param  pos  Index of first character to copy from.
     *  @param  n  Number of characters to copy.
     *  @param  a  Allocator to use.
     */
    basic_string(const basic_string &str, size_type pos, size_type n,
                 const Allocator &a)
        : M_dataplus(M_local_data(), a) {
        const CharType *start =
            str.M_data() +
            str.M_check(pos, "basic_string::basic_string out of range");
        M_construct(start, start + str.M_limit(pos, n),
                    easystl::forward_iterator_tag());
    }

    /**
     *  @brief  Construct string initialized by a character %array.
     *  @param  s  Source character %array.
     *  @param  n  Number of characters to copy.
     *  @param  a  Allocator to use (default is default allocator).
     */
    basic_string(const CharType *s, size_type n,
                 const Allocator &a = Allocator())
        : M_dataplus(M_local_data(), a) {
        if (s == 0 && n > 0) {
            throw std::logic_error(
                "basic_string: construction from null is not valid");
        }
        M_construct(s, s + n, easystl::forward_iterator_tag());
    }

    /**
     *  @brief  Construct string as copy of a C string.
     *  @param  s  Source C string.
     *  @param  a  Allocator to use (default is default allocator).
     */
    basic_string(const CharType *s, const Allocator &a = Allocator())
        : M_dataplus(M_local_data(), a) {
        // NB: Not required, but considered best practice.
        if (s == 0)
            throw std::logic_error(
                ("basic_string: construction from null is not valid"));
        const CharType *end = s + traits_type::length(s);
        M_construct(s, end, easystl::forward_iterator_tag());
    }

    /**
     *  @brief  Construct string as multiple characters.
     *  @param  n  Number of characters.
     *  @param  c  Character to use.
     *  @param  a  Allocator to use (default is default allocator).
     */
    basic_string(size_type n, CharType c, const Allocator &a = Allocator())
        : M_dataplus(M_local_data(), a) {
        M_construct(n, c);
    }

    /**
     *  @brief  Move construct string.
     *  @param  str  Source string.
     *
     *  The newly-created string contains the exact contents of @a str.
     *  @a str is a valid, but unspecified string.
     */
    basic_string(basic_string &&str) noexcept
        : M_dataplus(M_local_data(), std::move(str.M_get_allocator())) {
        if (str.M_is_local()) {
            M_init_local_buf();
            traits_type::copy(M_local_buf, str.M_local_buf, str.length() + 1);
        } else {
            M_data(str.M_data());
            M_capacity(str.M_allocated_capacity);
        }

        M_length(str.length());
        str.M_data(str.M_use_local_data());
        str.M_set_length(0);
    }

    /**
     *  @brief  Construct string from an initializer %list.
     *  @param  l  std::initializer_list of characters.
     *  @param  a  Allocator to use (default is default allocator).
     */
    basic_string(std::initializer_list<CharType> l,
                 const Allocator &a = Allocator())
        : M_dataplus(M_local_data(), a) {
        M_construct(l.begin(), l.end(), easystl::forward_iterator_tag());
    }

    /**
     *  @brief  Copy constructor with allocator.
     *  @param  str  Source string.
     *  @param  a  Allocator to use (default is default allocator).
     */
    basic_string(basic_string &str, const Allocator &a)
        : M_dataplus(M_local_data(), a) {
        M_construct(str.being(), str.end(), easystl::forward_iterator_tag());
    }

    /**
     *  @brief  Move constructor with allocator.
     *  @param  str  Source string.
     *  @param  a  Allocator to use (default is default allocator).
     *
     *  The newly-created string contains the exact contents of @a str.
     *  @a str is a valid, but unspecified string.
     */
    basic_string(basic_string &&str,
                 const Allocator &a) noexcept(alloc_traits::S_always_equal())
        : M_dataplus(M_local_data(), a) {
        if (str.M_is_local()) {
            M_init_local_buf();
            traits_type::copy(M_local_buf, str.M_local_buf, str.length() + 1);
            M_length(str.length());
            str.M_set_length(0);
        } else if (alloc_traits::S_always_equal() || str.get_allocator() == a) {
            M_data(str.M_data());
            M_length(str.length());
            M_capacity(str.M_allocated_capacity);
            str.M_data(str.M_use_local_data());
            str.M_set_length(0);
        } else {
            M_construct(str.begin(), str.end(),
                        easystl::forward_iterator_tag());
        }
    }

    /**
     *  @brief  Construct string as copy of a range.
     *  @param  __beg  Start of range.
     *  @param  __end  End of range.
     *  @param  __a  Allocator to use (default is default allocator).
     */
    template <typename InputIterator,
              typename std::enable_if<
                  easystl::is_input_iterator<InputIterator>::value,
                  bool>::type = true>
    basic_string(InputIterator beg, InputIterator end,
                 const Allocator &a = Allocator())
        : M_dataplus(M_local_data(), a), M_string_length(0) {
        M_construct(beg, end, easystl::iterator_category(beg));
    }

    ~basic_string() { M_dispose(); }

    /**
     *  @brief  Assign the value of @a str to this string.
     *  @param  str  Source string.
     */
    basic_string &operator=(const basic_string &str) {
        return this->assign(str);
    }

    /**
     *  @brief  Copy contents of @a s into this string.
     *  @param  s  Source null-terminated string.
     */
    basic_string &operator=(const CharType *s) { return this->assign(s); }

    /**
     *  @brief  Set value to string of length 1.
     *  @param  c  Source character.
     *
     *  Assigning to a character makes this string length 1 and
     *  (*this)[0] == @a c.
     */
    basic_string &operator=(CharType c) {
        this->assign(1, c);
        return *this;
    }

    /**
     *  @brief  Move assign the value of @a str to this string.
     *  @param  str  Source string.
     *
     *  The contents of @a str are moved into this string (without copying).
     *  @a str is a valid, but unspecified string.
     */
    // _GLIBCXX_RESOLVE_LIB_DEFECTS
    // 2063. Contradictory requirements for string move assignment
    basic_string &
    operator=(basic_string &&str) noexcept(alloc_traits::S_nothrow_move()) {
        const bool equal_allocs = alloc_traits::S_always_equal() ||
                                  M_get_allocator() == str.M_get_allocator();
        if (!M_is_local() && alloc_traits::S_propagate_on_move_assign() &&
            !equal_allocs) {
            // Destroy existing storage before replacing allocator.
            M_destroy(M_allocated_capacity);
            M_data(M_local_data());
            M_set_length(0);
        }
        // Replace allocator if POCMA is true.
        std::__alloc_on_move(M_get_allocator(), str.M_get_allocator());

        if (str.M_is_local()) {
            // We've always got room for a short string, just copy it
            // (unless this is a self-move, because that would violate the
            // char_traits::copy precondition that the ranges don't overlap).
            if (std::addressof(str) != this) {
                if (str.size())
                    this->S_copy(M_data(), str.M_data(), str.size());
                M_set_length(str.size());
            }
        } else if (alloc_traits::S_propagate_on_move_assign() || equal_allocs) {
            // Just move the allocated pointer, our allocator can free it.
            pointer data = nullptr;
            size_type capacity;
            if (!M_is_local()) {
                if (equal_allocs) {
                    // __str can reuse our existing storage.
                    data = M_data();
                    capacity = M_allocated_capacity;
                } else // __str can't use it, so free it.
                    M_destroy(M_allocated_capacity);
            }

            M_data(str.M_data());
            M_length(str.length());
            M_capacity(str.M_allocated_capacity);
            if (data) {
                str.M_data(data);
                str.M_capacity(capacity);
            } else
                str.M_data(str.M_use_local_data());
        } else // Need to do a deep copy
        {
            assign(str);
        }
        str.clear();
        return *this;
    }

    /**
     *  @brief  Set value to string constructed from initializer %list.
     *  @param  l  std::initializer_list.
     */
    basic_string &operator=(std::initializer_list<CharType> l) {
        this->assign(l.begin(), l.size());
        return *this;
    }

    // Iterators:
    /**
     *  Returns a read/write iterator that points to the first character in
     *  the %string.
     */
    iterator begin() noexcept { return iterator(M_data()); }

    /**
     *  Returns a read-only (constant) iterator that points to the first
     *  character in the %string.
     */
    const_iterator begin() const noexcept { return const_iterator(M_data()); }

    /**
     *  Returns a read/write iterator that points one past the last
     *  character in the %string.
     */
    iterator end() noexcept { return iterator(M_data() + this->size()); }

    /**
     *  Returns a read-only (constant) iterator that points one past the
     *  last character in the %string.
     */
    const_iterator end() const noexcept {
        return const_iterator(M_data() + this->size());
    }

    /**
     *  Returns a read/write reverse iterator that points to the last
     *  character in the %string.  Iteration is done in reverse element
     *  order.
     */
    reverse_iterator rbegin() noexcept { return reverse_iterator(this->end()); }

    /**
     *  Returns a read-only (constant) reverse iterator that points
     *  to the last character in the %string.  Iteration is done in
     *  reverse element order.
     */
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(this->end());
    }

    /**
     *  Returns a read/write reverse iterator that points to one before the
     *  first character in the %string.  Iteration is done in reverse
     *  element order.
     */
    reverse_iterator rend() noexcept { return reverse_iterator(this->begin()); }

    /**
     *  Returns a read-only (constant) reverse iterator that points
     *  to one before the first character in the %string.  Iteration
     *  is done in reverse element order.
     */
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(this->begin());
    }

    /**
     *  Returns a read-only (constant) iterator that points to the first
     *  character in the %string.
     */
    const_iterator cbegin() const noexcept {
        return const_iterator(this->_M_data());
    }

    /**
     *  Returns a read-only (constant) iterator that points one past the
     *  last character in the %string.
     */
    const_iterator cend() const noexcept {
        return const_iterator(this->_M_data() + this->size());
    }

    /**
     *  Returns a read-only (constant) reverse iterator that points
     *  to the last character in the %string.  Iteration is done in
     *  reverse element order.
     */
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(this->end());
    }

    /**
     *  Returns a read-only (constant) reverse iterator that points
     *  to one before the first character in the %string.  Iteration
     *  is done in reverse element order.
     */
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(this->begin());
    }

  public:
    // Capacity
    /*  Returns the number of characters in the string, not including any
     *  null-termination.
     */
    size_type size() const noexcept { return M_string_length; }

    /*  Returns the number of characters in the string, not including any
     *  null-termination.
     */
    size_type length() const noexcept { return M_string_length; }

    ///  Returns the size() of the largest possible %string.
    size_type max_size() const noexcept {
        return (alloc_traits::max_size(M_get_allocator()) - 1) / 2;
    }

    /**
     *  @brief  Resizes the %string to the specified number of characters.
     *  @param  n  Number of characters the %string should contain.
     *  @param  c  Character to fill any new elements.
     *
     *  This function will %resize the %string to the specified
     *  number of characters.  If the number is smaller than the
     *  %string's current size the %string is truncated, otherwise
     *  the %string is extended and new elements are %set to @a __c.
     */
    void resize(size_type n, CharType c);

    /**
     *  @brief  Resizes the %string to the specified number of characters.
     *  @param  __n  Number of characters the %string should contain.
     *
     *  This function will resize the %string to the specified length.  If
     *  the new size is smaller than the %string's current size the %string
     *  is truncated, otherwise the %string is extended and new characters
     *  are default-constructed.  For basic types such as char, this means
     *  setting them to 0.
     */
    void resize(size_type n) { this->resize(n, CharType()); }

    void shrink_to_fit() noexcept { this->reserve(); }

    /**
     *  Returns the total number of characters that the %string can hold
     *  before needing to allocate more memory.
     */
    size_type capacity() const noexcept {
        return M_is_local() ? size_type(S_local_capacity)
                            : M_allocated_capacity;
    }

    /**
     *  @brief  Attempt to preallocate enough memory for specified number of
     *          characters.
     *  @param  __res_arg  Number of characters required.
     *  @throw  std::length_error  If @a __res_arg exceeds @c max_size().
     *
     *  This function attempts to reserve enough memory for the
     *  %string to hold the specified number of characters.  If the
     *  number requested is more than max_size(), length_error is
     *  thrown.
     *
     *  The advantage of this function is that if optimal code is a
     *  necessity and the user can determine the string length that will be
     *  required, the user can reserve the memory in %advance, and thus
     *  prevent a possible reallocation of memory and copying of %string
     *  data.
     */
    void reserve(size_type res_arg);

    /**
     *  Erases the string, making it empty.
     */
    void clear() noexcept { M_set_length(0); }

    /**
     *  Returns true if the %string is empty.  Equivalent to
     *  <code>*this == ""</code>.
     */
    bool empty() const noexcept { return this->size() == 0; }

    // Element access:
    /**
     *  @brief  Subscript access to the data contained in the %string.
     *  @param  __pos  The index of the character to access.
     *  @return  Read-only (constant) reference to the character.
     *
     *  This operator allows for easy, array-style, data access.
     *  Note that data access with this operator is unchecked and
     *  out_of_range lookups are not defined. (For checked lookups
     *  see at().)
     */
    const_reference operator[](size_type pos) const noexcept {
        EASYSTL_DEBUG(pos <= size());
        return M_data()[pos];
    }

    /**
     *  @brief  Subscript access to the data contained in the %string.
     *  @param  __pos  The index of the character to access.
     *  @return  Read/write reference to the character.
     *
     *  This operator allows for easy, array-style, data access.
     *  Note that data access with this operator is unchecked and
     *  out_of_range lookups are not defined. (For checked lookups
     *  see at().)
     */
    reference operator[](size_type pos) {
        // Allow pos == size() both in C++98 mode, as v3 extension,
        // and in C++11 mode.
        EASYSTL_DEBUG(pos <= size());
        return M_data()[pos];
    }

    /**
     *  @brief  Provides access to the data contained in the %string.
     *  @param __n The index of the character to access.
     *  @return  Read-only (const) reference to the character.
     *  @throw  std::out_of_range  If @a n is an invalid index.
     *
     *  This function provides for safer data access.  The parameter is
     *  first checked that it is in the range of the string.  The function
     *  throws out_of_range if the check fails.
     */
    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(n >= this->size(), "basic_string::at()");
        return M_data()[n];
    }
    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(n >= this->size(), "basic_string::at()");
        return M_data()[n];
    }

    /**
     *  Returns a read/write reference to the data at the first
     *  element of the %string.
     */
    reference front() noexcept {
        EASYSTL_DEBUG(!empty());
        return operator[](0);
    }

    const_reference front() const noexcept {
        EASYSTL_DEBUG(!empty());
        return operator[](0);
    }

    /**
     *  Returns a read/write reference to the data at the last
     *  element of the %string.
     */
    reference back() noexcept {
        EASYSTL_DEBUG(!empty());
        return operator[](this->size() - 1);
    }

    const_reference back() const noexcept {
        EASYSTL_DEBUG(!empty());
        return operator[](this->size() - 1);
    }

    template <class InputIterator,
              typename std::enable_if<
                  easystl::is_input_iterator<InputIterator>::value,
                  bool>::type = true>
    basic_string &append(InputIterator first, InputIterator last) {
        // TODO:
    }

    /**
     *  @brief  附加多个字符
     *  @param  n  附加字符的数量
     *  @param  c  附加的字符
     *  @return  该字符串的引用
     */
    basic_string &append(size_type n, CharType c) {
        return M_replace_aux(this->size(), size_type(0), n, c);
    }

    /**
     *  @brief  Set value to contents of another string.
     *  @param  __str  Source string to use.
     *  @return  Reference to this string.
     */
    basic_string &assign(const basic_string &str) {
        if (alloc_traits::S_propagate_on_copy_assign()) {
            if (!alloc_traits::S_always_equal() && !M_is_local() &&
                M_get_allocator() != str.M_get_allocator()) {
                // Propagating allocator cannot free existing storage so must
                // deallocate it before replacing current allocator.
                if (str.size() <= S_local_capacity) {
                    M_destroy(M_allocated_capacity);
                    M_data(M_use_local_data());
                    M_set_length(0);
                } else {
                    const auto len = str.size();
                    auto alloc = str.M_get_allocator();
                    // If this allocation throws there are no effects:
                    auto ptr = S_allocate(alloc, len + 1);
                    M_destroy(M_allocated_capacity);
                    M_data(ptr);
                    M_capacity(len);
                    M_set_length(len);
                }
            }
            // TODO: replace std
            std::__alloc_on_copy(M_get_allocator(), str.M_get_allocator());
        }
        this->M_assign(str);
        return *this;
    }

    /**
     *  @brief  Set value to contents of another string.
     *  @param  str  Source string to use.
     *  @return  Reference to this string.
     *
     *  This function sets this string to the exact contents of @a __str.
     *  @a __str is a valid, but unspecified string.
     */
    basic_string &
    assign(basic_string &&str) noexcept(alloc_traits::S_nothrow_move()) {
        return *this = easystl::move(str);
    }

    /**
     *  @brief  Set value to a substring of a string.
     *  @param str  The string to use.
     *  @param pos  Index of the first character of str.
     *  @param n  Number of characters to use.
     *  @return  Reference to this string.
     *  @throw  std::out_of_range if @a pos is not a valid index.
     *
     *  This function sets this string to the substring of @a __str
     *  consisting of @a __n characters at @a __pos.  If @a __n is
     *  is larger than the number of available characters in @a
     *  __str, the remainder of @a __str is used.
     */
    basic_string &assign(const basic_string &str, size_type pos,
                         size_type n = npos) {
        return M_replace(size_type(0), this->size(),
                         str.M_data() +
                             str.M_check(pos, "basic_string::assign"),
                         str.M_limit(pos, n));
    }

    /**
     *  @brief  Set value to a C substring.
     *  @param s  The C string to use.
     *  @param n  Number of characters to use.
     *  @return  Reference to this string.
     *
     *  This function sets the value of this string to the first @a __n
     *  characters of @a __s.  If @a __n is is larger than the number of
     *  available characters in @a __s, the remainder of @a __s is used.
     */
    basic_string &assign(const CharType *s, size_type n) {
        // __glibcxx_requires_string_len(__s, __n);
        THROW_LOGIC_ERROR_IF(s == nullptr, "basic_string::assign");
        THROW_LENGTH_ERROR_IF(n > traits_type::length(s),
                              "basic_string::assign");
        return M_replace(size_type(0), this->size(), s, n);
    }

    /**
     *  @brief  Set value to contents of a C string.
     *  @param  s  The C string to use.
     *  @return  Reference to this string.
     *
     *  This function sets the value of this string to the value of @a __s.
     *  The data is copied, so there is no dependence on @a __s once the
     *  function returns.
     */
    basic_string &assign(const CharType *s) {
        // __glibcxx_requires_string(__s);
        THROW_LOGIC_ERROR_IF(s == nullptr, "basic_string::assign");
        return M_replace(size_type(0), this->size(), s, traits_type::length(s));
    }

    /**
     *  @brief  Set value to multiple characters.
     *  @param n  Length of the resulting string.
     *  @param c  The character to use.
     *  @return  Reference to this string.
     *
     *  This function sets the value of this string to @a __n copies of
     *  character @a __c.
     */
    basic_string &assign(size_type n, CharType c) {
        return M_replace_aux(size_type(0), this->size(), n, c);
    }

    /**
     *  @brief  Set value to a range of characters.
     *  @param __first  Iterator referencing the first character to append.
     *  @param __last  Iterator marking the end of the range.
     *  @return  Reference to this string.
     *
     *  Sets value of string to characters in the range [__first,__last).
     */
    template <
        class InputIterator,
        typename std::enable_if<
            easystl::is_input_iterator<InputIterator>::value, int>::type = 0>
    basic_string &assign(InputIterator first, InputIterator last) {
        if (std::__is_one_of<InputIterator, const_iterator, iterator,
                             const CharType *, CharType *>::value) {
            __glibcxx_requires_valid_range(first, last);
            return M_replace(size_type(0), size(), easystl::to_address(first),
                             last - first);
        } else
            return *this = basic_string(first, last, get_allocator());
    }

    /**
     *  @brief  Set value to an initializer_list of characters.
     *  @param __l  The initializer_list of characters to assign.
     *  @return  Reference to this string.
     */
    basic_string &assign(std::initializer_list<CharType> l) {
        // The initializer_list array cannot alias the characters in *this
        // so we don't need to use replace to that case.
        const size_type n = l.size();
        if (n > capacity())
            *this = basic_string(l.begin(), l.end(), get_allocator());
        else {
            if (n)
                S_copy(M_data(), l.begin(), n);
            M_set_length(n);
        }
        return *this;
    }

    /**
     *  @brief  以另一字符串替换部分字符串
     *  @param  pos  第一个需替换的字符的索引
     *  @param  n1  被替换的字符的数量
     *  @param  str  插入的字符串
     *  @return  此字符串的引用
     */
    basic_string &replace(size_type pos, size_type n, const basic_string &str) {
        return this->replace(pos, n, str.M_data(), str.size());
    }

    /**
     *  @brief  以另一字符串的部分替换部分字符串
     *  @param  pos  第一个需替换的字符的索引
     *  @param  n1  被替换的字符的数量
     *  @param  str  插入的字符串
     *  @param  pos2  第一个插入的字符的索引
     *  @param  n2  插入的字符的数量
     *  @return  此字符串的引用
     */
    basic_string &replace(size_type pos1, size_type n1, const basic_string &str,
                          size_type pos2, size_type n2 = npos) {
        return this->replace(
            pos1, n1, str.M_data() + str.M_check(pos2, "basic_string::replace"),
            str.M_limit(pos2, n2));
    }

    /**
     *  @brief  以部分 C 风格字符串替换部分字符串
     *  @param  pos  第一个需替换的字符的索引
     *  @param  n1  被替换的字符的数量
     *  @param  s  C 风格字符串指针
     *  @param  n2  插入的字符的数量
     *  @return  此字符串的引用
     */
    basic_string &replace(size_type pos, size_type n1, const CharType *s,
                          size_type n2) {
        THROW_LOGIC_ERROR_IF(s == nullptr, "basic_string::relpace");
        if (n2 > traits_type::length(s)) {
            n2 = traits_type::length(s);
        }
        return M_replace(M_check(pos, "basic_string::replace"),
                         M_limit(pos, n1), s, n2);
    }

    /**
     *  @brief  以 C 风格字符串替换部分字符串
     *  @param  pos  第一个需替换的字符的索引
     *  @param  n1  被替换的字符的数量
     *  @param  s  C 风格字符串指针
     *  @return  此字符串的引用
     */
    basic_string &replace(size_type pos, size_type n1, const CharType *s) {
        THROW_LOGIC_ERROR_IF(s == nullptr, "basic_string::relpace");
        return this->replace(pos, n1, s, traits_type::length(s));
    }

    /**
     *  @brief  以多个字符替换子字符串
     *  @param  pos  第一个需替换的字符的索引
     *  @param  n1  被替换的字符的数量
     *  @param  n2  插入的字符的数量
     *  @param  c  插入的字符
     *  @return  此字符串的引用
     */
    basic_string &replace(size_type pos, size_type n1, size_type n2,
                          CharType c) {
        return M_replace_aux(M_check(pos, "basic_string::replace"),
                             M_limit(pos, n1), n2, c);
    }

    /**
     *  @brief  以字符串替换范围内字符串
     *  @param  iter1  指向范围起始位置的迭代器
     *  @param  iter2  指向范围结束位置的迭代器
     *  @param  str  插入的字符串
     *  @return  此字符串的引用
     */
    basic_string &replace(const_iterator iter1, const_iterator iter2,
                          const basic_string &str) {
        return this->replace(iter1, iter2, str.M_data(), str.size());
    }

    /**
     *  @brief  以部分 C 风格字符串替换范围内字符串
     *  @param  iter1  指向范围起始位置的迭代器
     *  @param  iter2  指向范围结束位置的迭代器
     *  @param  s  C 风格字符串指针
     *  @param  n  插入的字符的数量
     *  @return  此字符串的引用
     */
    basic_string &replace(const_iterator iter1, const_iterator iter2,
                          const CharType *s, size_type n) {
        EASYSTL_DEBUG(iter1 >= begin() && iter1 <= iter2 && iter2 <= end());
        return this->replace(iter1 - begin(), iter2 - iter1, s, n);
    }

    /**
     *  @brief  以 C 风格字符串替换范围内字符串
     *  @param  iter1  指向范围起始位置的迭代器
     *  @param  iter2  指向范围结束位置的迭代器
     *  @param  s  C 风格字符串指针
     *  @return  此字符串的引用
     */
    basic_string &replace(const_iterator iter1, const_iterator iter2,
                          const CharType *s) {
        return this->replace(iter1, iter2, s, traits_type::length(s));
    }

    /**
     *  @brief  以多个字符替换范围内子字符串
     *  @param  iter1  范围起始位置指针
     *  @param  iter2  范围结束位置指针
     *  @param  n  插入的字符的数量
     *  @param  c  插入的字符
     *  @return  此字符串的引用
     */
    basic_string &replace(const_iterator iter1, const_iterator iter2,
                          size_type n, CharType c) {
        EASYSTL_DEBUG(iter1 >= begin() && iter1 <= iter2 && iter2 <= end());
        return M_replace_aux(iter1 - begin(), iter2 - iter1, n, c);
    }

    template <
        typename InputIter,
        typename std::enable_if<easystl::is_input_iterator<InputIter>::vlaue,
                                bool>::type = true>
    basic_string &replace(const_iterator iter1, const_iterator iter2,
                          InputIter input_iter1, InputIter input_iter2) {
        EASYSTL_DEBUG(begin() <= iter1 && iter1 <= iter2 && iter2 <= end());
        return M_replace_dispatch(iter1, iter2, input_iter1, input_iter2,
                                  std::__false_type());
    }

    basic_string &replace(const_iterator iter1, const_iterator iter2,
                          CharType *p1, CharType *p2) {
        EASYSTL_DEBUG(begin() <= iter1 && iter1 <= iter2 && iter2 <= end());
        return this->replace(iter1 - begin(), iter2 - iter1, p1, p2 - p1);
    }

    basic_string &replace(const_iterator iter1, const_iterator iter2,
                          const CharType *p1, const CharType *p2) {
        EASYSTL_DEBUG(begin() <= iter1 && iter1 <= iter2 && iter2 <= end());
        return this->replace(iter1 - begin(), iter2 - iter1, p1, p2 - p1);
    }

    basic_string &replace(const_iterator iter1, const_iterator iter2,
                          iterator p1, iterator p2) {
        EASYSTL_DEBUG(begin() <= iter1 && iter1 <= iter2 && iter2 <= end());
        return this->replace(iter1 - begin(), iter2 - iter1, p1.base(),
                             p2 - p1);
    }

    basic_string &replace(const_iterator iter1, const_iterator iter2,
                          const_iterator p1, const_iterator p2) {
        EASYSTL_DEBUG(begin() <= iter1 && iter1 <= iter2 && iter2 <= end());
        return this->replace(iter1 - begin(), iter2 - iter1, p1.base(),
                             p2 - p1);
    }

    basic_string &replace(const_iterator iter1, const_iterator iter2,
                          std::initializer_list<CharType> l) {
        return this->replace(iter1, iter2, l.begin(), l.end());
    }

  private:
    template <typename Integer>
    basic_string &M_replace_dispatch(const_iterator iter1, const_iterator iter2,
                                     Integer n, Integer val, std::__true_type) {
        return M_replace_aux(iter1 - begin(), iter2 - iter1, n, val);
    }

    template <typename InputIterator>
    basic_string &M_replace_dispatch(const_iterator iter1, const_iterator iter2,
                                     InputIterator input_iter1,
                                     InputIterator input_iter2,
                                     std::__false_type) {
        const basic_string s(input_iter1, input_iter2, this->get_allocator());
        const size_type n = iter2 - iter1;
        return M_replace(iter1 - begin(), n, s.M_data(), s.size());
    }

    basic_string &M_replace(size_type pos, size_type len1, const CharType *s,
                            const size_type len2);

    void M_replace_cold(pointer p, size_type len1, const CharType *s,
                        const size_type len2, const size_type how_much);

    basic_string &M_replace_aux(size_type pos1, size_type n1, size_type n2,
                                CharType c);

  public:
    const CharType *data() const noexcept { return M_data(); }

    allocator_type get_allocator() const noexcept { return M_get_allocator(); }
};

template <typename CharType, typename CharTraits, typename Allocator>
typename basic_string<CharType, CharTraits, Allocator>::pointer
basic_string<CharType, CharTraits, Allocator>::M_create(
    size_type &capacity, size_type old_capacity) {
    THROW_LENGTH_ERROR_IF(capacity > max_size(), "basic_string::M_create");

    if (capacity > old_capacity && capacity < 2 * old_capacity) {
        capacity = 2 * old_capacity;
        if (capacity > max_size()) {
            capacity = max_size();
        }
    }
    return S_allocate(M_get_allocator(), capacity + 1);
}

template <typename CharType, typename CharTraits, typename Allocator>
template <typename InputIter>
void basic_string<CharType, CharTraits, Allocator>::M_construct(
    InputIter first, InputIter end, easystl::input_iterator_tag) {
    size_type len = 0;
    size_type capacity = size_type(S_local_capacity);

    M_init_local_buf();

    while (first != end && len < capacity) {
        M_local_buf[len++] = *first;
        ++first;
    }

    struct Guard {
        basic_string *guarded;
        explicit Guard(basic_string *s) : guarded(s) {}

        ~Guard() {
            if (guarded) {
                guarded->M_dispose();
            }
        }

    } guard(this);

    while (first != end) {
        if (len == capacity) {
            capacity = len + 1;
            pointer another = M_create(capacity, len);
            this->S_copy(another, M_data(), len);
            M_dispose();
            M_data(another);
            M_capacity(capacity);
        }
        traits_type::assign(M_data()[len++], *first);
        ++first;
    }

    guard.guarded = 0;
    M_set_length(len);
}

/**
 *  @brief  Helper function to construct a basic_string from a range
 *  @param first  starting iterator
 *  @param end  end iterator
 *  @param  forward_iterator_tag
 */
template <typename CharType, typename CharTraits, typename Allocator>
template <typename InputIter>
void basic_string<CharType, CharTraits, Allocator>::M_construct(
    InputIter first, InputIter end, easystl::forward_iterator_tag) {
    size_type dnew = static_cast<size_type>(easystl::distance(first, end));

    if (dnew > size_type(S_local_capacity)) {
        M_data(M_create(dnew, size_type(0)));
        M_capacity(dnew);
    } else {
        M_init_local_buf();
    }

    struct Guard {
        basic_string *guarded;
        explicit Guard(basic_string *s) : guarded(s) {}

        ~Guard() {
            if (guarded) {
                guarded->M_dispose();
            }
        }

    } guard(this);

    this->S_copy_chars(M_data(), first, end);

    guard.guarded = 0;

    M_set_length(dnew);
}

/**
 *  @brief  Helper function to construct basic_string from a character.
 *  @param  n  Number of characters.
 *  @param  c  Character.
 */
template <typename CharType, typename CharTraits, typename Allocator>
void basic_string<CharType, CharTraits, Allocator>::M_construct(size_type n,
                                                                CharType c) {
    if (n > size_type(S_local_capacity)) {
        M_data(M_create(n, size_type(0)));
        M_capacity(n);
    } else {
        M_init_local_buf();
    }

    if (n) {
        this->S_assign(M_data(), n, c);
    }
    M_set_length(n);
}

template <typename CharType, typename CharTraits, typename Allocator>
void basic_string<CharType, CharTraits, Allocator>::M_assign(
    const basic_string &str) {
    if (this != easystl::address_of(str)) {
        const size_type rsize = str.length();
        const size_type current_capacity = capacity();

        if (rsize > current_capacity) {
            size_type new_capacity = rsize;
            pointer tmp = M_create(new_capacity, current_capacity);
            M_dispose();
            M_data(tmp);
            M_capacity(new_capacity);
        }

        if (rsize) {
            this->S_copy(M_data(), str.M_data(), rsize);
        }

        M_set_length(rsize);
    }
}

template <typename CharType, typename CharTraits, typename Allocator>
void basic_string<CharType, CharTraits, Allocator>::M_mutate(size_type pos,
                                                             size_type len1,
                                                             const CharType *s,
                                                             size_type len2) {
    const size_type how_much = length() - pos - len1;
    size_type new_capacity = length() + len2 - len1;
    pointer r = M_create(new_capacity, capacity());

    if (pos) {
        this->S_copy(r, M_data(), pos);
    }
    if (s && len2) {
        this->S_copy(r + pos, s, len2);
    }
    if (how_much) {
        this->S_copy(r + pos + len2, M_data() + pos + len1, how_much);
    }

    M_dispose();
    M_data(r);
    M_capacity(new_capacity);
}

template <typename CharType, typename CharTraits, typename Allocator>
void basic_string<CharType, CharTraits, Allocator>::M_erase(size_type pos,
                                                            size_type n) {
    const size_type how_much = length() - pos - n;

    if (how_much && n) {
        this->S_move(M_data() + pos, M_data() + pos + n, how_much);
        M_set_length(length() - n);
    }
}

// operator==
template <typename CharType, typename CharTraits, typename Allocator>
inline bool
operator==(const basic_string<CharType, CharTraits, Allocator> &lhs,
           const basic_string<CharType, CharTraits, Allocator> &rhs) noexcept {
    return lhs.size() == rhs.size() &&
           !CharTraits::compare(lhs.data(), rhs.data(), lhs.size());
}

// operator==
template <typename CharType, typename CharTraits, typename Allocator>
inline bool
operator!=(const basic_string<CharType, CharTraits, Allocator> &lhs,
           const basic_string<CharType, CharTraits, Allocator> &rhs) noexcept {
    return !(lhs == rhs);
}

/**
 *  @brief  Test equivalence of string and C string.
 *  @param __lhs  String.
 *  @param __rhs  C string.
 *  @return  True if @a __lhs.compare(@a __rhs) == 0.  False otherwise.
 */
template <typename CharType, typename CharTraits, typename Allocator>
inline bool operator==(const basic_string<CharType, CharTraits, Allocator> &lhs,
                       const CharType *rhs) {
    return lhs.size() == CharTraits::length(rhs) &&
           !CharTraits::compare(lhs.data(), rhs, lhs.size());
}

template <typename CharType, typename CharTraits, typename Allocator>
void basic_string<CharType, CharTraits, Allocator>::reserve(size_type res) {
    const size_type current_capacity = capacity();
    // _GLIBCXX_RESOLVE_LIB_DEFECTS
    // 2968. Inconsistencies between basic_string reserve and
    // vector/unordered_map/unordered_set reserve functions
    // P0966 reserve should not shrink
    if (res <= current_capacity)
        return;

    pointer tmp = M_create(res, current_capacity);
    this->S_copy(tmp, M_data(), length() + 1);
    M_dispose();
    M_data(tmp);
    M_capacity(res);
}

/**
 *  @brief  替换字符辅助函数
 *  @param  pos  第一个需要替换的字符的索引
 *  @param  n1  被替换的字符的数量
 *  @param  n2  插入的字符的数量
 *  @param  c  插入的字符
 *  @return  此字符串的引用
 */
template <typename CharType, typename CharTraits, typename Allocator>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::M_replace_aux(size_type pos1,
                                                             size_type n1,
                                                             size_type n2,
                                                             CharType c) {
    M_check_length(n1, n2, "basic_string::M_replace_aux");

    const size_type old_size = this->size();
    const size_type new_size = old_size + n2 - n1;

    if (new_size <= this->capacity()) {
        pointer p = this->M_data() + pos1;

        const size_type how_much = old_size - pos1 - n1;
        if (how_much && n1 != n2)
            this->S_move(p + n2, p + n1, how_much);
    } else
        this->M_mutate(pos1, n1, 0, n2);

    if (n2)
        this->S_assign(this->M_data() + pos1, n2, c);

    this->M_set_length(new_size);
    return *this;
}

/**
 *  @brief  Helper function for replacing string with overlap.
 *  @param  p  Pointer to the destination.
 *  @param  len1  Length of the portion to be replaced.
 *  @param  s  Pointer to the source.
 *  @param  len2  Length of the portion used to replace.
 *  @return  return
 */
template <typename CharType, typename CharTraits, typename Allocator>
void basic_string<CharType, CharTraits, Allocator>::M_replace_cold(
    pointer p, size_type len1, const CharType *s, const size_type len2,
    const size_type how_much) {
    // Work in-place.
    if (len2 && len2 <= len1)
        this->S_move(p, s, len2);
    if (how_much && len1 != len2)
        this->S_move(p + len2, p + len1, how_much);
    if (len2 > len1) {
        if (s + len2 <= p + len1)
            this->S_move(p, s, len2);
        else if (s >= p + len1) {
            // Hint to middle end that __p and __s overlap
            // (PR 98465).
            const size_type poff = (s - p) + (len2 - len1);
            this->S_copy(p, p + poff, len2);
        } else {
            const size_type nleft = (p + len1) - s;
            this->S_move(p, s, nleft);
            this->S_copy(p + nleft, p + len2, len2 - nleft);
        }
    }
}

/**
 *  @brief  Helper function for replacement, assignment.
 *  @param  pos  Index of first character to replace.
 *  @param  len1  Length of the portion to be replaced in the string.
 *  @param  s  Pointer to the source string.
 *  @param  len2  Length of the portion used to replace in the source string.
 *  @return  Reference of the original string
 */
template <typename CharType, typename CharTraits, typename Allocator>
basic_string<CharType, CharTraits, Allocator> &
basic_string<CharType, CharTraits, Allocator>::M_replace(size_type pos,
                                                         size_type len1,
                                                         const CharType *s,
                                                         const size_type len2) {
    M_check_length(len1, len2, "basic_string::M_replace");

    const size_type old_size = this->size();
    const size_type new_size = old_size + len2 - len1;

    if (new_size <= this->capacity()) {
        pointer p = this->M_data() + pos;

        const size_type how_much = old_size - pos - len1;
        // not overlap
        if (M_disjunct(s) == true) {
            if (how_much && len1 != len2)
                this->S_move(p + len2, p + len1, how_much);
            if (len2)
                this->S_copy(p, s, len2);
        } else
            // overlap
            M_replace_cold(p, len1, s, len2, how_much);
    } else
        this->M_mutate(pos, len1, s, len2);

    this->M_set_length(new_size);
    return *this;
}

} // namespace easystl

#endif // !EASYSTL_BASIC_STRING_H
