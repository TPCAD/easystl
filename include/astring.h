#ifndef EASYSTL_ASTRING_H_
#define EASYSTL_ASTRING_H_

// 定义了 string, wstring, u16string, u32string 类型

#include "basic_string.h"

namespace easystl {

using string = easystl::basic_string<char>;
using wstring = easystl::basic_string<wchar_t>;
using u16string = easystl::basic_string<char16_t>;
using u32string = easystl::basic_string<char32_t>;

} // namespace easystl
#endif // !EASYSTL_ASTRING_H_
