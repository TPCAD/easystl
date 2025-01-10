#ifndef EASYSTL_VECTOR_H
#define EASYSTL_VECTOR_H

#include "algobase.h"
#include "iterator/iterator_adapter.h"
#include "iterator/iterator_funcs.h"
#include "iterator/iterator_traits.h"
#include "memory/alloc_traits.h"
#include "memory/uninitialized.h"
#include "utility.h"
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <type_traits>

#include <iostream>

namespace easystl {

template <typename Tp, typename Alloc = easystl::allocator<Tp>> struct vector {
    static_assert(
        std::is_same<typename std::remove_cv<Tp>::type, Tp>::value,
        "easystl::vector must have a non-const, non-volatile value_type");

  public:
    // 重绑定分配器到 Tp
    // 例如 Alloc = easystl::allocator<double>，Tp = int
    // 那么 Tp_alloc_type = easystl::allocator<int>
    using Tp_alloc_type =
        typename easystl_cxx::alloc_traits<Alloc>::template rebind<Tp>::other;

    using pointer = typename easystl_cxx::alloc_traits<Tp_alloc_type>::pointer;
    using allocator_type = Alloc;

  private:
    struct alloc_hider : public Tp_alloc_type {
      public:
        pointer M_start;
        pointer M_finish;
        pointer M_end_of_storage;

        alloc_hider() noexcept(
            std::is_nothrow_default_constructible<Tp_alloc_type>::value)
            : Tp_alloc_type(), M_start(), M_finish(), M_end_of_storage() {}

        alloc_hider(const allocator_type &a) noexcept
            : Tp_alloc_type(a), M_start(), M_finish(), M_end_of_storage() {}

        alloc_hider(std::size_t n)
            : Tp_alloc_type(), M_start(), M_finish(), M_end_of_storage() {
            M_create_storage(n);
        }

        alloc_hider(std::size_t n, const allocator_type &a)
            : Tp_alloc_type(a), M_start(), M_finish(), M_end_of_storage() {
            M_create_storage(n);
        }

        alloc_hider(alloc_hider &&x) noexcept
            : Tp_alloc_type(easystl::move(x)), M_start(x.M_start),
              M_finish(x.M_finish), M_end_of_storage(x.M_end_of_storage) {
            x.M_start = pointer();
            x.M_finish = pointer();
            x.M_end_of_storage = pointer();
        }

        alloc_hider(Tp_alloc_type &&a) noexcept
            : Tp_alloc_type(easystl::move(a)), M_start(), M_finish(),
              M_end_of_storage() {}

        alloc_hider(alloc_hider &&x, const Tp_alloc_type &a) noexcept
            : Tp_alloc_type(a), M_start(), M_finish(), M_end_of_storage() {
            if (x.get_allocator() == a) {
                this->M_swap_data(x);
            } else {
                std::size_t n = x.M_finish - x.M_start;
                M_create_storage(n);
            }
        }

        alloc_hider(const Tp_alloc_type &a, alloc_hider &&x) noexcept
            : Tp_alloc_type(a), M_start(x.M_start), M_finish(x.M_finish),
              M_end_of_storage(x.M_end_of_storage) {
            x.M_start = pointer();
            x.M_finish = pointer();
            x.M_end_of_storage = pointer();
        }

        ~alloc_hider() noexcept {
            M_deallocate(M_start, M_end_of_storage - M_start);
        }

        void M_copy_data(const alloc_hider &a) noexcept {
            this->M_start = a.M_start;
            this->M_finish = a.M_finish;
            this->M_end_of_storage = a.M_end_of_storage;
        }

        void M_swap_data(alloc_hider &a) noexcept {
            alloc_hider tmp;
            tmp.M_copy_data(*this);
            this->M_copy_data(a);
            a.M_copy_data(tmp);
        }

        pointer M_allocate(std::size_t n) {
            using Tr = easystl_cxx::alloc_traits<Tp_alloc_type>;
            return n != 0 ? Tr::allocate(*this, n) : pointer();
        }

        void M_deallocate(pointer p, std::size_t n) {
            using Tr = easystl_cxx::alloc_traits<Tp_alloc_type>;
            if (p) {
                Tr::deallocate(*this, p, n);
            }
        }

        void M_create_storage(std::size_t n) {
            this->M_start = this->M_allocate(n);
            this->M_finish = this->M_start;
            this->M_end_of_storage = this->M_start + n;
        }
    };

    // 获取重绑定分配器
    Tp_alloc_type &M_get_Tp_allocator() noexcept { return this->M_data; }
    const Tp_alloc_type &M_get_Tp_allocator() const noexcept {
        return this->M_data;
    }

    // 获取未重绑定的分配器
    allocator_type get_allocator() const noexcept {
        return allocator_type(M_get_Tp_allocator());
    }

    // 数据指针，同时也是分配器
    alloc_hider M_data;

  public:
    using alloc_traits = easystl_cxx::alloc_traits<Tp_alloc_type>;
    using value_type = Tp;
    using const_pointer = typename alloc_traits::const_pointer;
    using reference = typename alloc_traits::reference;
    using const_reference = typename alloc_traits::const_reference;

    using iterator = easystl_cxx::normal_iterator<pointer, vector>;
    using const_iterator = easystl_cxx::normal_iterator<const_pointer, vector>;

    using reverse_iterator = easystl::reverse_iterator<iterator>;
    using const_reverse_iterator = easystl::reverse_iterator<const_iterator>;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

  public:
    /**
     *  @brief  创建不包含元素的 vector
     */
    vector() = default;

    /**
     *  @brief  创建不包含元素的 vector
     *  @param  a  分配器
     */
    explicit vector(const allocator_type &a) : M_data(a) {}

    /**
     *  @brief  创建包含 n 个默认初始化元素的 vector
     *  @param  n  元素数量
     *  @param  a  分配器
     */
    explicit vector(size_type n, const allocator_type &a = allocator_type())
        : M_data(S_check_init_len(n, a), a) {
        M_default_initialize(n);
    }

    /**
     *  @brief  创建包含 n 个 value 的 vector
     *  @param  n  元素数量
     *  @param  value  拷贝对象
     *  @param  a  分配器
     */
    vector(size_type n, value_type &value,
           const allocator_type &a = allocator_type())
        : M_data(S_check_init_len(n, a), a) {
        M_fill_initialize(n, value);
    }

    /**
     *  @brief  拷贝构造
     *  @param  v  vector
     */
    vector(const vector &v)
        : M_data(v.size(),
                 alloc_traits::S_select_on_copy(v.M_get_Tp_allocator())) {
        this->M_data.M_finish = easystl::__uninitialized_copy_a(
            v.begin(), v.end(), this->M_data.M_start, M_get_Tp_allocator());
    }

    /**
     *  @brief  移动构造
     *  @param  v  vector
     */
    vector(vector &&v) noexcept = default;

    /**
     *  @brief  带分配器的拷贝构造
     *  @param  v  vector
     *  @param  a  分配器
     */
    vector(const vector &v, const std::__type_identity_t<allocator_type> &a)
        : M_data(v.size(), a) {
        this->M_data.M_finish = easystl::__uninitialized_copy_a(
            v.begin(), v.end(), this->M_data.M_start, M_get_Tp_allocator());
    }

  private:
    vector(vector &&rv, const allocator_type &a, std::false_type) : M_data(a) {
        if (rv.get_allocator() == a) {
            this->M_data.M_swap_data(rv.M_data);
        } else if (!rv.empty()) {
            this->M_data.M_create_storage(rv.size());
            this->M_data.M_finish = easystl::__uninitialized_move_a(
                rv.begin(), rv.end(), rv.M_data.M_start, M_get_Tp_allocator());
            rv.clear();
        }
    }
    vector(vector &&rv, const allocator_type &a, std::true_type)
        : M_data(a, easystl::move(rv.M_data)) {}

  public:
    vector(
        vector &&rv,
        const std::__type_identity_t<allocator_type>
            &a) noexcept(noexcept(vector(std::declval<vector &&>(),
                                         std::declval<const allocator_type &>(),
                                         std::declval<typename alloc_traits::
                                                          is_always_equal>())

                                      ))
        : vector(easystl::move(rv), a,
                 typename alloc_traits::is_always_equal{}) {}

    /**
     *  @brief  使用初始化队列构造 vector
     *  @param  l  初始化队列
     */
    vector(std::initializer_list<value_type> l,
           const allocator_type &a = allocator_type())
        : M_data(a) {
        M_range_initialize(l.begin(), l.end(),
                           easystl::random_access_iterator_tag());
    }

    /**
     *  @brief  从范围中构造 vector
     *  @param  first  输入迭代器
     *  @param  last  输入迭代器
     *  @param  a  分配器
     */
    template <typename InputIterator,
              typename = easystl::RequireInputIter<InputIterator>>
    vector(InputIterator first, InputIterator last,
           const allocator_type &a = allocator_type())
        : M_data(a) {
        M_range_initialize(first, last, easystl::iterator_category(first));
    }

    /**
     *  @brief  析构函数
     *
     *  析构函数只负责删除元素，如果元素是指针，析构函数不会删除其所指向的内存
     */
    ~vector() noexcept {
        std::_Destroy(this->M_data.M_start, this->M_data.M_finish,
                      this->M_get_Tp_allocator());
    }

    /**
     *  @brief  拷贝赋值函数
     *  @param  x  具有相同元素类型和分配器类型的 vector
     *
     *  拷贝所有元素，未使用的容量不会被拷贝
     */
    vector &operator=(const vector &x);

    vector &operator=(vector &&x) noexcept(alloc_traits::S_nothrow_move()) {
        constexpr bool move_storage =
            alloc_traits::S_propagate_on_move_assign() ||
            alloc_traits::S_always_equal();
        M_move_assign(easystl::move(x), std::__bool_constant<move_storage>());
        return *this;
    }

    vector &operator=(std::initializer_list<value_type> l) {
        this->M_assign_aux(l.begin(), l.end(),
                           easystl::random_access_iterator_tag());
        return *this;
    }

    [[nodiscard]] size_type size() const noexcept {
        return size_type(M_data.M_finish - M_data.M_start);
    }
    [[nodiscard]] size_type max_size() const noexcept {
        return S_max_size(M_get_Tp_allocator());
    }

    /// normal iterator
    [[nodiscard]] iterator begin() noexcept { return iterator(M_data.M_start); }
    [[nodiscard]] const_iterator begin() const noexcept {
        return const_iterator(M_data.M_start);
    }
    [[nodiscard]] iterator end() noexcept { return iterator(M_data.M_finish); }
    [[nodiscard]] const_iterator end() const noexcept {
        return const_iterator(M_data.M_finish);
    }

    /// reverse iterator
    [[nodiscard]] reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    [[nodiscard]] reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    [[nodiscard]] reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    [[nodiscard]] const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    /// const iterator
    [[nodiscard]] const_iterator cbegin() const noexcept {
        return const_iterator(this->M_data.M_start);
    }
    [[nodiscard]] const_iterator cend() const noexcept {
        return const_iterator(this->M_data.M_finish);
    }
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    [[nodiscard]] const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // TODO: M_default_append is not defined
    void resize(size_type new_size) {
        if (new_size > size()) {
            M_default_append(new_size - size());
        } else {
            M_erase_at_end(this->M_data.M_start + new_size);
        }
    }

    bool empty() const noexcept { return begin() == end(); }

    void clear() noexcept { M_erase_at_end(this->M_data.M_start); }

  protected:
    void M_default_initialize(size_type n) {
        this->M_data.M_finish = easystl::uninitialized_default_n_a(
            this->M_data.M_start, n, M_get_Tp_allocator());
    }

    template <typename InputIterator>
    void M_range_initialize(InputIterator first, InputIterator last,
                            easystl::input_iterator_tag) {
        try {
            for (; first != last; ++first) {
                // BUG: emplace_back is not exist
                emplace_back(*first);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    template <typename ForwardIterator>
    void M_range_initialize(ForwardIterator first, ForwardIterator last,
                            easystl::forward_iterator_tag) {
        const size_type n = easystl::distance(first, last);
        this->M_data.M_start =
            this->M_data.M_allocate(S_check_init_len(n, M_get_Tp_allocator()));
        this->M_data.M_end_of_storage = this->M_data.M_start + n;
        this->M_data.M_finish = easystl::__uninitialized_copy_a(
            first, last, this->M_data.M_start, M_get_Tp_allocator());
    }

    void M_fill_initialize(size_type n, const value_type &value) {
        this->M_data.M_finish = easystl::uninitialized_fill_n_a(
            this->M_data.M_start, n, value, M_get_Tp_allocator());
    }

    template <typename InputIterator>
    void M_assign_aux(InputIterator first, InputIterator last,
                      easystl::input_iterator_tag) {
        pointer cur(this->M_data.M_start);
        for (; first != last && cur != this->M_data.M_finish;
             ++cur, (void)++first) {
            *cur = *first;
        }
        if (first == last) {
            M_erase_at_end(cur);
        } else {
            M_range_insert();
        }
    }

    template <typename InputIterator>
    void M_range_insert(iterator pos, InputIterator first, InputIterator last,
                        easystl::input_iterator_tag) {}

    // called by resize(n)
    void M_default_append(size_type n);

    /**
     *  @brief  删除 [pos, M_data.M_finish) 的元素
     *  @param  pos  指向新的结束位置的指针
     */
    void M_erase_at_end(pointer pos) noexcept {
        if (this->M_data.M_finish - pos) {
            std::_Destroy(pos, this->M_data.M_finish, M_get_Tp_allocator());
            this->M_data.M_finish = pos;
        }
    }

    size_type M_check_len(size_type n, const char *s) const {
        if (max_size() - size_type() < n) {
            throw std::length_error(s);
        }

        const size_type len = size() + easystl::max(size(), n);
        return (len < size() || len > max_size()) ? max_size() : len;
    }

    static size_type S_check_init_len(size_type n, const allocator_type &a) {
        if (n > S_max_size(Tp_alloc_type(a))) {
            throw std::length_error(
                "cannot create easystl::vector larger than max_size()");
        }
        return n;
    }
    static size_type S_max_size(const allocator_type &a) {
        const auto diff_max =
            std::numeric_limits<ptrdiff_t>::max() / sizeof(Tp);
        const auto alloc_max = alloc_traits::max_size(a);
        return easystl::min(diff_max, alloc_max);
    }

    void M_move_assign(vector &&x, std::true_type) noexcept {
        vector tmp(get_allocator());
        this->M_data.M_swap_data(x.M_data);
        tmp.M_data.M_swap_data(x.M_data);
        easystl::__alloc_on_move(M_get_Tp_allocator(), x.M_get_Tp_allocator());
    }
};

/**
 *  @brief  追加容量
 *  @param  n  需要追加的容量
 */
template <typename Tp, typename Alloc>
void vector<Tp, Alloc>::M_default_append(size_type n) {
    if (n == 0) {
        return;
    }

    const size_type csize = size();
    size_type avail_storage =
        size_type(this->M_data.M_end_of_storage - this->M_data.M_finish);

    if (avail_storage >= n) {
        this->M_data.M_finish = easystl::uninitialized_default_n_a(
            this->M_data.M_finish, n, M_get_Tp_allocator());
    } else {
        pointer old_start = this->M_data.M_start;
        pointer old_finish = this->M_data.M_finish;

        const size_type len = M_check_len(n, "vector::M_default_append");
        pointer new_start(this->M_data.M_allocate(len));

        struct Guard {
            pointer M_storage;
            size_type M_len;
            Tp_alloc_type &M_alloc;

            Guard(pointer s, size_type l, Tp_alloc_type &a)
                : M_storage(s), M_len(l), M_alloc(a) {}
            ~Guard() {
                if (M_storage) {
                    easystl_cxx::alloc_traits<Tp_alloc_type>::deallocate(
                        M_alloc, M_storage, M_len);
                }
            }

          private:
            Guard(const Guard &);
        };

        {
            Guard guard(new_start, len, this->M_data);
        }
    }
}

} // namespace easystl

#endif // !EASYSTL_VECTOR_H
