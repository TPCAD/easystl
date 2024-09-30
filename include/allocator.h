#ifndef EASYSTL_ALLOCATOR_H
#define EASYSTL_ALLOCATOR_H

#include "construct.h"

namespace easystl {

template <class T> class allocator {
  public:
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

  public:
    allocator() noexcept {}
    allocator(const allocator &) noexcept {}
    template <typename T1> allocator(const allocator<T1> &) noexcept {}
    ~allocator() noexcept {}

    static T *allocate();
    static T *allocate(size_type n);

    static void deallocate(T *ptr);
    static void deallocate(T *ptr, size_type n);

    static void construct(T *ptr);
    static void construct(T *ptr, const T &value);
    static void construct(T *ptr, T &&value);

    template <class... Args> static void construct(T *ptr, Args &&...args);

    static void destroy(T *ptr);
    static void destroy(T *first, T *last);
};

template <class T> T *allocator<T>::allocate() {
    return static_cast<T *>(::operator new(sizeof(T)));
}

template <class T> T *allocator<T>::allocate(size_type n) {
    if (n == 0) {
        return nullptr;
    }
    return static_cast<T *>(::operator new(n * sizeof(T)));
}

template <class T> void allocator<T>::deallocate(T *ptr) {
    if (ptr == nullptr) {
        return;
    }
    ::operator delete(ptr);
}

template <class T> void allocator<T>::deallocate(T *ptr, size_type) {
    if (ptr == nullptr) {
        return;
    }
    ::operator delete(ptr);
}

template <class T> void allocator<T>::construct(T *ptr, const T &value) {
    easystl::construct(ptr, value);
}

template <class T> void allocator<T>::construct(T *ptr, T &&value) {
    easystl::construct(ptr, move(value));
}

template <class T>
template <class... Args>
void allocator<T>::construct(T *ptr, Args &&...args) {
    easystl::construct(ptr, forward<Args>(args)...);
}

template <class T> void allocator<T>::destroy(T *ptr) { easystl::destroy(ptr); }

template <class T> void allocator<T>::destroy(T *first, T *last) {
    easystl::destroy(first, last);
}

// FIX: strange
template <typename T1, typename T2>
inline bool operator!=(const allocator<T1> &, const allocator<T2> &) noexcept {
    return false;
}

// FIX: strange
template <typename T1, typename T2>
inline bool operator==(const allocator<T1> &, const allocator<T2> &) noexcept {
    return true;
}

} // namespace easystl

#endif // !EASYSTL_ALLOCATOR_H
