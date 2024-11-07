#ifndef EASYSTL_PTR_TRAITS_H
#define EASYSTL_PTR_TRAITS_H

namespace easystl {

template <typename T> T *to_address(T *p) noexcept { return p; }

template <typename Ptr>
auto to_address(const Ptr &p) noexcept -> decltype(p.operator->()) {
    return p.operator->();
}

} // namespace easystl
#endif // !EASYSTL_PTR_TRAITS_H
