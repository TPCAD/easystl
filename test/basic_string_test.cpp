#include "char_traits.h"
#include "stringfwd.h"
#include "utility.h"
#include "gtest/gtest.h"
#include <climits>
#include <cstring>
#include <initializer_list>
#include <list>
#include <string>

// 1. basic_string()
namespace constructor_test {
TEST(BasicStringDefaultConstructorTest, DefaultConstructor) {
    // 1. basic_string()
    easystl::string str;
    // empty string
    EXPECT_EQ(str, "");
    EXPECT_TRUE(str.empty());
    EXPECT_EQ(str.size(), 0);
    EXPECT_EQ(str.length(), 0);
    // capacity is not 0
    EXPECT_GT(str.capacity(), 0);
    // data is no null
    EXPECT_NE(str.data(), nullptr);

    // nothrow constructible
    EXPECT_TRUE(std::is_nothrow_default_constructible<easystl::string>::value);

    // immediate assignment
    // TODO: assign()
    // str = "hello world";
    // EXPECT_EQ(str, "hello world");
}
} // namespace constructor_test

// 2. basic_string(const Allocator &a)

// 3. basic_string(const basic_string &str)
namespace constructor_test {
TEST(BasicStringCopyConstructorTest, CopiesEmptyString) {
    easystl::string original;
    easystl::string copy(original);

    EXPECT_TRUE(copy.empty());
    EXPECT_EQ(copy.length(), 0);
}

TEST(BasicStringCopyConstructorTest, CopiesShortString) {
    easystl::string original("Short");
    easystl::string copy(original);

    EXPECT_EQ(copy, original);
    EXPECT_EQ(copy.length(), original.length());
}

TEST(BasicStringCopyConstructorTest, CopiesLongString) {
    easystl::string original(1000, 'a'); // likely to be heap-allocated
    easystl::string copy(original);

    EXPECT_EQ(copy, original);
    EXPECT_EQ(copy.length(), original.length());
}

TEST(BasicStringCopyConstructorTest, CreatesSeparateObject) {
    easystl::string original("Test");
    easystl::string copy(original);

    original[0] = 'B';
    EXPECT_EQ(original, "Best");
    EXPECT_EQ(copy, "Test");
}

TEST(BasicStringCopyConstructorTest, CopiesCapacity) {
    easystl::string original;
    original.reserve(100);
    easystl::string copy(original);

    EXPECT_LE(copy.capacity(), original.capacity());
}

TEST(BasicStringCopyConstructorTest, CopiesNullCharacters) {
    easystl::string original("ab\0cd", 5); // String with null character
    easystl::string copy(original);

    EXPECT_EQ(copy.length(), 5);
    EXPECT_EQ(easystl::string(copy.data(), 5), easystl::string("ab\0cd", 5));
}
} // namespace constructor_test

// 4. basic_string(const basic_string &str, size_type pos, const Allocator &a =
// Allocator())
namespace constructor_test {
TEST(BasicStringSubstringConstructorTest, ConstructFromBeginning) {
    easystl::string original("Hello, World!");
    easystl::string substring(original, 0);

    EXPECT_EQ(substring, original);
}

TEST(BasicStringSubstringConstructorTest, ConstructFromMiddle) {
    easystl::string original("Hello, World!");
    easystl::string substring(original, 7);

    EXPECT_EQ(substring, "World!");
}

TEST(BasicStringSubstringConstructorTest, ConstructEmptyString) {
    easystl::string original("Hello");
    easystl::string substring(original, 5);

    EXPECT_TRUE(substring.empty());
}

TEST(BasicStringSubstringConstructorTest, ThrowsOutOfRange) {
    easystl::string original("Hello");

    EXPECT_THROW(
        { easystl::string substring(original, 10); }, std::out_of_range);
}

TEST(BasicStringSubstringConstructorTest, HandlesNullCharacters) {
    easystl::string original("ab\0cd", 5);
    easystl::string substring(original, 2);

    EXPECT_EQ(substring.length(), 3);
    EXPECT_EQ(easystl::string(substring.data(), 3), easystl::string("\0cd", 3));
}
} // namespace constructor_test

// 5. basic_string(const basic_string &str, size_type pos, size_type n)
namespace constructor_test {
TEST(BasicStringSubstringConstructorTest, CopySubstring) {
    // Test case 1: Copy a substring from the beginning
    easystl::string str1("Hello, world!");
    easystl::string str2(str1, 0, 5);
    EXPECT_EQ(str2, "Hello");

    // Test case 2: Copy a substring from the middle
    easystl::string str3("Hello, world!");
    easystl::string str4(str3, 7, 5);
    EXPECT_EQ(str4, "world");

    // Test case 3: Copy a substring that spans the entire string
    easystl::string str5("Hello, world!");
    easystl::string str6(str5, 0, str5.length());
    EXPECT_EQ(str6, str5);

    // Test case 4: Copy an empty substring
    easystl::string str7("Hello, world!");
    easystl::string str8(str7, 10, 0);
    EXPECT_EQ(str8, "");

    // Test case 5: Handle out-of-bounds index
    easystl::string str9("Hello, world!");
    EXPECT_THROW(easystl::string(str9, 15, 2), std::out_of_range);

    // Test case 6: Handle negative index
    easystl::string str10("Hello, world!");
    EXPECT_THROW(easystl::string(str10, -1, 2), std::out_of_range);

    // Test case 7: Handle negative length
    easystl::string str11("Hello, world!");
    EXPECT_EQ(easystl::string(str11, 0, -1), str11);
}
} // namespace constructor_test

// 6. basic_string(const basic_string &str, size_type pos, size_type n,
// const Allocator &a)

// 7. basic_string(const CharType *s, size_type n,
//              const Allocator &a = Allocator())
namespace constructor_test {
TEST(BasicStringCharArrayConstructorTest, ConstructFromEmptyArray) {
    const char *empty = "";
    easystl::string str(empty, 0);

    EXPECT_TRUE(str.empty());
    EXPECT_EQ(str.length(), 0);
}

TEST(BasicStringCharArrayConstructorTest, ConstructFromNonEmptyArray) {
    const char *hello = "Hello, World!";
    easystl::string str(hello, 5);

    EXPECT_EQ(str, "Hello");
    EXPECT_EQ(str.length(), 5);
}

TEST(BasicStringCharArrayConstructorTest, ConstructWithNullCharacter) {
    const char with_null[] = {'H', 'e', 'l', 'l', 'o', '\0',
                              'W', 'o', 'r', 'l', 'd'};
    easystl::string str(with_null, 11);

    EXPECT_EQ(str.length(), 11);
    EXPECT_EQ(str, easystl::string("Hello\0World", 11));
}

TEST(BasicStringCharArrayConstructorTest,
     ThrowsOnNullPointerWithNonZeroLength) {
    EXPECT_THROW({ easystl::string str(nullptr, 5); }, std::logic_error);
}

TEST(BasicStringCharArrayConstructorTest, AllowsNullPointerWithZeroLength) {
    EXPECT_NO_THROW({
        easystl::string str(nullptr, 0);
        EXPECT_TRUE(str.empty());
    });
}

TEST(BasicStringCharArrayConstructorTest, HandlesLongArray) {
    easystl::string long_str(1000, 'a');
    easystl::string str(long_str.data(), 1000);

    EXPECT_EQ(str, long_str);
    EXPECT_EQ(str.length(), 1000);
}

TEST(BasicStringCharArrayConstructorTest,
     ConstructsWithLengthLessThanArraySize) {
    const char *hello = "Hello, World!";
    easystl::string str(hello, 7);

    EXPECT_EQ(str, "Hello, ");
    EXPECT_EQ(str.length(), 7);
}

TEST(BasicStringCharArrayConstructorTest, ConstructsCorrectCapacity) {
    const char *hello = "Hello, World!";
    easystl::string str(hello, 13);

    EXPECT_GE(str.capacity(), str.length());
}
} // namespace constructor_test

// 8. basic_string(const CharType *s, const Allocator &a = Allocator())
namespace constructor_test {
TEST(BasicStringCStringConstructorTest, ConstructFromEmptyCString) {
    const char *empty = "";
    easystl::string str(empty);

    EXPECT_TRUE(str.empty());
    EXPECT_EQ(str.length(), 0);
}

TEST(BasicStringCStringConstructorTest, ConstructFromNonEmptyCString) {
    const char *hello = "Hello, World!";
    easystl::string str(hello);

    EXPECT_EQ(str, "Hello, World!");
    EXPECT_EQ(str.length(), 13);
}

TEST(BasicStringCStringConstructorTest, ConstructWithNullCharacter) {
    const char *with_null = "Hello\0World";
    easystl::string str(with_null);

    EXPECT_EQ(str, "Hello");
    EXPECT_EQ(str.length(), 5);
}

TEST(BasicStringCStringConstructorTest, ThrowsOnNullPointer) {
    EXPECT_THROW({ easystl::string str(nullptr); }, std::logic_error);
}

TEST(BasicStringCStringConstructorTest, HandlesLongString) {
    easystl::string long_str(1000, 'a');
    const char *long_cstr = long_str.data();

    easystl::string str(long_cstr);

    EXPECT_EQ(str, long_str);
    EXPECT_EQ(str.length(), 1000);
}

TEST(BasicStringCStringConstructorTest, ConstructsCorrectCapacity) {
    const char *hello = "Hello, World!";
    easystl::string str(hello);

    EXPECT_GE(str.capacity(), str.length());
}

} // namespace constructor_test

// 9. basic_string(size_type n, CharType c, const Allocator &a = Allocator())
namespace constructor_test {
TEST(BasicStringMultipleCharConstructorTest, ConstructWithMultipleCharacters) {
    easystl::string str1(5, 'a');
    EXPECT_EQ(str1.size(), 5);
    EXPECT_EQ(str1, "aaaaa");

    // construct empty string
    easystl::string str2(0, 'x');
    EXPECT_TRUE(str2.empty());
    EXPECT_EQ(str2.size(), 0);

    // large string
    const size_t large_size = 1000000;
    easystl::string str3(large_size, 'b');
    EXPECT_EQ(str3.size(), large_size);
    EXPECT_EQ(str3[0], 'b');
    EXPECT_EQ(str3[large_size - 1], 'b');

    // non-printable character
    easystl::string str4(3, '\0');
    EXPECT_EQ(str4.size(), 3);
    EXPECT_EQ(str4[0], '\0');
    EXPECT_EQ(str4[1], '\0');
    EXPECT_EQ(str4[2], '\0');
}
} // namespace constructor_test

// 10. basic_string(basic_string &&str) noexcept
namespace constructor_test {
TEST(BasicStringMoveConstructorTest, MovesShortString) {
    easystl::string original("Short");
    easystl::string moved(easystl::move(original));

    EXPECT_EQ(moved, "Short");
    EXPECT_TRUE(original.empty()); // NOLINT: intentional use after move
}

TEST(BasicStringMoveConstructorTest, MovesLongString) {
    easystl::string original(1000, 'a'); // likely to be heap-allocated
    easystl::string moved(easystl::move(original));

    EXPECT_EQ(moved.length(), 1000);
    EXPECT_EQ(moved, easystl::string(1000, 'a'));
    EXPECT_TRUE(original.empty()); // NOLINT: intentional use after move
}

TEST(BasicStringMoveConstructorTest, NoThrowGuarantee) {
    EXPECT_TRUE(std::is_nothrow_move_constructible<easystl::string>::value);
}

TEST(BasicStringMoveConstructorTest, PreservesCapacity) {
    easystl::string original;
    original.reserve(1000);
    size_t originalCapacity = original.capacity();

    easystl::string moved(easystl::move(original));

    // do not preserve capacity
    EXPECT_LE(moved.capacity(), originalCapacity);
}

TEST(BasicStringMoveConstructorTest, OriginalBecomesValidButUnspecified) {
    easystl::string original("Test");
    easystl::string moved(easystl::move(original));

    // Original should be in a valid but unspecified state
    EXPECT_NO_THROW(original.clear()); // NOLINT: intentional use after move
    // EXPECT_NO_THROW(original =
    //                     "New content"); // NOLINT: intentional use after move
}
} // namespace constructor_test

// 11. basic_string(std::initializer_list<CharType> l,
//                  const Allocator &a = Allocator())
namespace constructor_test {
TEST(BasicStringTest, ConstructFromInitializerList) {
    // Test construction with default allocator
    easystl::string s1({'H', 'e', 'l', 'l', 'o'});
    EXPECT_EQ(s1, "Hello");
    EXPECT_EQ(s1.length(), 5);

    // Test empty initializer list
    easystl::string s2({});
    EXPECT_TRUE(s2.empty());
    EXPECT_EQ(s2.length(), 0);

    // Test with longer string
    easystl::string s4(
        {'L', 'o', 'n', 'g', 'e', 'r', ' ', 's', 't', 'r', 'i', 'n', 'g'});
    EXPECT_EQ(s4, "Longer string");
    EXPECT_EQ(s4.length(), 13);

    // Test with special characters
    easystl::string s5({'\0', '\n', '\t'});
    EXPECT_EQ(s5.length(), 3);
    EXPECT_EQ(s5[0], '\0');
    EXPECT_EQ(s5[1], '\n');
    EXPECT_EQ(s5[2], '\t');
}
} // namespace constructor_test

// 12. basic_string(basic_string &str, const Allocator &a)

// 13. basic_string(basic_string &&str,
//                  const Allocator &a) noexcept(alloc_traits::S_always_equal())

// 1. basic_string &assign(const basic_string &str)
namespace assignment_test {
TEST(BasicStringAssignTest, AssignFromAnotherString) {
    easystl::string s1("Good");
    easystl::string s2("World");

    // Test assigning from another string
    s1.assign(s2);
    EXPECT_EQ(s1, "World");
    EXPECT_EQ(s1.length(), 5);

    // Test self-assignment
    s1.assign(s1);
    EXPECT_EQ(s1, "World");
    EXPECT_EQ(s1.length(), 5);

    // Test assigning from an empty string
    easystl::string empty;
    s1.assign(empty);
    EXPECT_TRUE(s1.empty());
    EXPECT_EQ(s1.length(), 0);

    // Test assigning a long string
    easystl::string long_str(1000, 'a');
    s1.assign(long_str);
    EXPECT_EQ(s1, long_str);
    EXPECT_EQ(s1.length(), 1000);

    // Test assigning a string that's longer than the local buffer
    // but shorter than the current capacity
    s1 = easystl::string(100, 'b');
    easystl::string medium_str(500, 'c');
    s1.assign(medium_str);
    EXPECT_EQ(s1, medium_str);
    EXPECT_EQ(s1.length(), 500);
}

TEST(BasicStringAssignTest, AssignWithCustomAllocator) {
    using CustomAllocator =
        std::allocator<char>; // For simplicity, we're using std::allocator
    using CustomString =
        std::basic_string<char, std::char_traits<char>, CustomAllocator>;

    CustomAllocator alloc1;
    CustomAllocator alloc2;

    CustomString s1("Hello", alloc1);
    CustomString s2("World", alloc2);

    s1.assign(s2);
    EXPECT_EQ(s1, "World");
    EXPECT_EQ(s1.length(), 5);
    // Note: In a real scenario with truly custom allocators, you might want to
    // check if the allocator has been correctly propagated here.
}

} // namespace assignment_test

// 2. basic_string &assign(basic_string &&str)
//                         noexcept(alloc_traits::S_nothrow_move())
namespace assignment_test {
TEST(BasicStringMoveAssignTest, MoveAssignFromAnotherString) {
    easystl::string s1("Good");
    easystl::string s2("World");

    // Test move assigning from another string
    s1.assign(easystl::move(s2));
    EXPECT_EQ(s1, "World");
    EXPECT_EQ(s1.length(), 5);
    // s2 should be in a valid but unspecified state
    // We can't make assumptions about its content, but it should be usable
    EXPECT_NO_THROW(s2.clear());

    // Test move assigning from an empty string
    easystl::string empty;
    s1.assign(easystl::move(empty));
    EXPECT_TRUE(s1.empty());
    EXPECT_EQ(s1.length(), 0);

    // Test move assigning a long string
    easystl::string long_str(1000, 'a');
    s1.assign(easystl::move(long_str));
    EXPECT_EQ(s1.length(), 1000);
    EXPECT_EQ(s1, easystl::string(1000, 'a'));

    // Test self-move-assignment (this is usually undefined behavior, but
    // let's
    // test it doesn't crash)
    EXPECT_NO_THROW(s1.assign(std::move(s1)));
    // We can't make assumptions about the content after self-move-assignment
    // but the string should still be in a valid state
    EXPECT_NO_THROW(s1.clear());
}
} // namespace assignment_test

// 3. basic_string &assign(const basic_string &str, size_type pos, size_type n =
//                         npos) {
namespace assignment_test {
TEST(BasicStringSubstringAssignTest, AssignSubstring) {
    easystl::string source("Hello, World!");
    easystl::string target;

    // Test assigning a substring
    target.assign(source, 7, 5);
    EXPECT_EQ(target, "World");

    // Test assigning from the beginning
    target.assign(source, 0, 5);
    EXPECT_EQ(target, "Hello");

    // Test assigning to the end (using npos)
    target.assign(source, 7);
    EXPECT_EQ(target, "World!");

    // Test assigning more characters than available
    target.assign(source, 7, 100);
    EXPECT_EQ(target, "World!");

    // Test assigning an empty substring
    target.assign(source, 13, 5);
    EXPECT_TRUE(target.empty());

    // Test self-assignment of substring
    source.assign(source, 7, 5);
    EXPECT_EQ(source, "World");
}

TEST(BasicStringSubstringAssignTest, AssignSubstringExceptions) {
    easystl::string source("Hello, World!");
    easystl::string target;

    // Test assigning with invalid position
    EXPECT_THROW(target.assign(source, 14, 1), std::out_of_range);

    // Test assigning with position equal to size
    EXPECT_NO_THROW(target.assign(source, 13, 0));
    EXPECT_TRUE(target.empty());
}

TEST(BasicStringSubstringAssignTest, AssignSubstringEdgeCases) {
    easystl::string source("a");
    easystl::string target;

    // Test with a single-character string
    target.assign(source, 0, 1);
    EXPECT_EQ(target, "a");

    // Test with a single-character string, requesting more
    target.assign(source, 0, 5);
    EXPECT_EQ(target, "a");

    // Test with an empty string
    easystl::string empty;
    target.assign(empty, 0, 5);
    EXPECT_TRUE(target.empty());
}

TEST(BasicStringSubstringAssignTest, AssignSubstringPerformance) {
    // This test is to ensure that the implementation is efficient
    // for large strings. It's not a strict performance test, but
    // it should complete quickly if the implementation is correct.
    easystl::string source(1000000, 'a'); // 1 million 'a's
    easystl::string target;

    EXPECT_NO_THROW({ target.assign(source, 500000, 250000); });
    EXPECT_EQ(target.size(), 250000);
    EXPECT_EQ(target, easystl::string(250000, 'a'));
}
} // namespace assignment_test

// 4. basic_string &assign(const CharType *s, size_type n)
namespace assignment_test {
TEST(BasicStringCStringAssignTest, AssignCString) {
    easystl::string target;
    const char *source = "Hello, World!";

    // Test assigning part of a C string
    target.assign(source, 5);
    EXPECT_EQ(target, "Hello");

    // Test assigning entire C string
    target.assign(source, 13);
    EXPECT_EQ(target, "Hello, World!");

    // Test assigning empty string
    target.assign(source, 0);
    EXPECT_TRUE(target.empty());

    // Test assigning single character
    target.assign(source, 1);
    EXPECT_EQ(target, "H");
}

TEST(BasicStringCStringAssignTest, AssignCStringExceptions) {
    easystl::string target;
    const char *source = "Hello, World!";

    // Test assigning with length equal to string length
    EXPECT_NO_THROW(target.assign(source, 13));
    EXPECT_EQ(target, source);

    // Test assigning with length greater than string length
    EXPECT_THROW(target.assign(source, 20), std::length_error);
}

TEST(BasicStringCStringAssignTest, AssignCStringEdgeCases) {
    easystl::string target;

    // Test with empty C string
    const char *empty = "";
    EXPECT_NO_THROW(target.assign(empty, 0));
    EXPECT_TRUE(target.empty());

    // Test with null pointer (this should ideally cause undefined behavior,
    // but many implementations treat it as an empty string)
    // Uncomment if your implementation is expected to handle this gracefully
    EXPECT_THROW(target.assign(nullptr, 0), std::logic_error);
}

TEST(BasicStringCStringAssignTest, AssignCStringPerformance) {
    // This test is to ensure that the implementation is efficient
    // for large strings. It's not a strict performance test, but
    // it should complete quickly if the implementation is correct.
    easystl::string target;
    char *large_source = new char[1000001];
    std::fill_n(large_source, 1000000, 'a');
    large_source[1000000] = '\0';

    EXPECT_NO_THROW({ target.assign(large_source, 1000000); });
    EXPECT_EQ(target.size(), 1000000);
    EXPECT_EQ(target, easystl::string(1000000, 'a'));

    delete[] large_source;
}
} // namespace assignment_test

// 5. basic_string &assign(const CharType *s)
namespace assignment_test {
TEST(BasicStringFullCStringAssignTest, AssignFullCString) {
    easystl::string target;
    const char *source = "Hello, World!";

    // Test assigning full C string
    target.assign(source);
    EXPECT_EQ(target, "Hello, World!");
    EXPECT_EQ(target.length(), 13);

    // Test assigning empty C string
    const char *empty = "";
    target.assign(empty);
    EXPECT_TRUE(target.empty());
    EXPECT_EQ(target.length(), 0);

    // Test assigning single character C string
    const char *single = "X";
    target.assign(single);
    EXPECT_EQ(target, "X");
    EXPECT_EQ(target.length(), 1);

    // Test self-assignment (implementation defined, but should not crash)
    target = "Test";
    const char *self = target.data();
    EXPECT_NO_THROW(target.assign(self));
    EXPECT_EQ(target, "Test");
}

TEST(BasicStringFullCStringAssignTest, AssignFullCStringExceptions) {
    easystl::string target;

    // Test assigning nullptr (should throw std::logic_error)
    EXPECT_THROW(target.assign(nullptr), std::logic_error);
}

TEST(BasicStringFullCStringAssignTest, AssignFullCStringPerformance) {
    // This test is to ensure that the implementation is efficient
    // for large strings. It's not a strict performance test, but
    // it should complete quickly if the implementation is correct.
    easystl::string target;
    char *large_source = new char[1000001];
    std::fill_n(large_source, 1000000, 'a');
    large_source[1000000] = '\0';

    EXPECT_NO_THROW({ target.assign(large_source); });
    EXPECT_EQ(target.size(), 1000000);
    EXPECT_EQ(target, easystl::string(1000000, 'a'));

    delete[] large_source;
}

TEST(BasicStringFullCStringAssignTest, AssignFullCStringMultipleOperations) {
    easystl::string target;

    // Test multiple assign operations
    target.assign("First");
    EXPECT_EQ(target, "First");

    target.assign("Second String");
    EXPECT_EQ(target, "Second String");

    target.assign("Third");
    EXPECT_EQ(target, "Third");

    // Test assign after other operations
    // target += " Additional";
    // target.assign("Overwritten");
    // EXPECT_EQ(target, "Overwritten");
}
} // namespace assignment_test

// 6. basic_string &assign(size_type n, CharType c)
namespace assignment_test {
TEST(BasicStringRepeatedCharAssignTest, AssignRepeatedChar) {
    easystl::string target;

    // Test assigning multiple copies of a character
    target.assign(5, 'a');
    EXPECT_EQ(target, "aaaaa");
    EXPECT_EQ(target.length(), 5);

    // Test assigning single character
    target.assign(1, 'b');
    EXPECT_EQ(target, "b");
    EXPECT_EQ(target.length(), 1);

    // Test assigning zero characters (should result in empty string)
    target.assign(0, 'c');
    EXPECT_TRUE(target.empty());
    EXPECT_EQ(target.length(), 0);

    // Test assigning non-printable character
    target.assign(3, '\0');
    EXPECT_EQ(target.length(), 3);
    for (char c : target) {
        EXPECT_EQ(c, '\0');
    }
}

TEST(BasicStringRepeatedCharAssignTest, AssignRepeatedCharLargeCount) {
    easystl::string target;
    size_t large_count = 1000000; // 1 million

    // Test assigning a large number of characters
    EXPECT_NO_THROW({ target.assign(large_count, 'x'); });
    EXPECT_EQ(target.length(), large_count);
    EXPECT_EQ(target, easystl::string(large_count, 'x'));
}

TEST(BasicStringRepeatedCharAssignTest, AssignRepeatedCharEdgeCases) {
    easystl::string target;

    // Test assigning max possible size_t value (this might throw
    // std::length_error or bad_alloc)
    size_t max_size_t = std::numeric_limits<size_t>::max();
    EXPECT_THROW(
        { target.assign(max_size_t, 'a'); },
        std::exception); // We use std::exception as the exact exception type
                         // might vary

    // Test assigning after other operations
    target = "Initial";
    target.assign(3, 'd');
    EXPECT_EQ(target, "ddd");
}

TEST(BasicStringRepeatedCharAssignTest, AssignRepeatedCharMultipleOperations) {
    easystl::string target;

    // Test multiple assign operations
    target.assign(5, 'a');
    EXPECT_EQ(target, "aaaaa");

    target.assign(3, 'b');
    EXPECT_EQ(target, "bbb");

    target.assign(1, 'c');
    EXPECT_EQ(target, "c");

    target.assign(0, 'd');
    EXPECT_TRUE(target.empty());

    // Test assign after other operations
    // target += "Additional";
    // target.assign(2, 'e');
    // EXPECT_EQ(target, "ee");
}
} // namespace assignment_test

// 7. basic_string(InputIterator beg, InputIterator end, const Allocator &a =
// Allocator())
// TODO: no other iterator container for testing
namespace assignment_test {
// TEST(BasicStringIteratorRangeAssignTest, AssignFromVector) {
//     easystl::string target;
//     std::vector<char> source = {'H', 'e', 'l', 'l', 'o'};
//
//     target.assign(source.begin(), source.end());
//     EXPECT_EQ(target, "Hello");
// }
//
// TEST(BasicStringIteratorRangeAssignTest, AssignFromList) {
//     easystl::string target;
//     std::list<char> source = {'W', 'o', 'r', 'l', 'd'};
//
//     target.assign(source.begin(), source.end());
//     EXPECT_EQ(target, "World");
// }
//
// TEST(BasicStringIteratorRangeAssignTest, AssignFromCArray) {
//     easystl::string target;
//     const char source[] = "Test";
//
//     target.assign(std::begin(source),
//                   std::end(source) - 1); // Exclude null terminator
//     EXPECT_EQ(target, "Test");
// }
//
// TEST(BasicStringIteratorRangeAssignTest, AssignFromAnotherString) {
//     easystl::string target;
//     easystl::string source = "Another";
//
//     target.assign(source.begin(), source.end());
//     EXPECT_EQ(target, "Another");
// }
//
// TEST(BasicStringIteratorRangeAssignTest, AssignEmptyRange) {
//     easystl::string target = "Initial";
//     std::vector<char> empty;
//
//     target.assign(empty.begin(), empty.end());
//     EXPECT_TRUE(target.empty());
// }
//
// TEST(BasicStringIteratorRangeAssignTest, AssignSubrange) {
//     easystl::string target;
//     std::vector<char> source = {'S', 'u', 'b', 'r', 'a', 'n', 'g', 'e'};
//
//     target.assign(source.begin() + 2, source.begin() + 5);
//     EXPECT_EQ(target, "bra");
// }
//
// TEST(BasicStringIteratorRangeAssignTest, AssignWithNonContiguousContainer) {
//     easystl::string target;
//     std::list<char> source = {'N', 'o', 'n', 'C', 'o', 'n', 't', 'i', 'g'};
//
//     target.assign(source.begin(), source.end());
//     EXPECT_EQ(target, "NonContig");
// }
//
// TEST(BasicStringIteratorRangeAssignTest, AssignLargeRange) {
//     easystl::string target;
//     std::vector<char> large_source(1000000, 'a');
//
//     EXPECT_NO_THROW(
//         { target.assign(large_source.begin(), large_source.end()); });
//     EXPECT_EQ(target.length(), 1000000);
//     EXPECT_EQ(target, easystl::string(1000000, 'a'));
// }
//
// TEST(BasicStringIteratorRangeAssignTest, AssignWithMixedCharTypes) {
//     easystl::string target;
//     std::vector<unsigned char> source = {65, 66, 67, 68,
//                                          69}; // ASCII values for A, B, C, D,
//                                          E
//
//     target.assign(source.begin(), source.end());
//     EXPECT_EQ(target, "ABCDE");
// }
//
// TEST(BasicStringIteratorRangeAssignTest, AssignMultipleOperations) {
//     easystl::string target = "Initial";
//     std::vector<char> source1 = {'F', 'i', 'r', 's', 't'};
//     std::list<char> source2 = {'S', 'e', 'c', 'o', 'n', 'd'};
//
//     target.assign(target.begin(), target.end());
//     EXPECT_EQ(target, "First");
//
//     target.assign(source2.begin(), source2.end());
//     EXPECT_EQ(target, "Second");
//
//     target.assign(target.begin() + 1, target.end() - 1);
//     EXPECT_EQ(target, "econ");
// }
// } // namespace assignment_test

//
// TEST(BasicStringMoveAssignTest, MoveAssignWithCustomAllocator) {
//     using CustomAllocator =
//         std::allocator<char>; // For simplicity, we're using std::allocator
//     using CustomString =
//         std::basic_string<char, std::char_traits<char>, CustomAllocator>;
//
//     CustomAllocator alloc1;
//     CustomAllocator alloc2;
//
//     CustomString s1("Hello", alloc1);
//     CustomString s2("World", alloc2);
//
//     s1.assign(std::move(s2));
//     EXPECT_EQ(s1, "World");
//     EXPECT_EQ(s1.length(), 5);
//     // Note: In a real scenario with truly custom allocators, you might want
//     to
//     // check if the allocator has been correctly handled here.
// }
//
// // This test checks if the noexcept specification is correct
// TEST(BasicStringMoveAssignTest, NoexceptSpecification) {
//     EXPECT_TRUE(noexcept(std::declval<easystl::string &>().assign(
//         std::declval<easystl::string &&>())));
// }
} // namespace assignment_test

// 8. basic_string &assign(std::initializer_list<CharType> l)
namespace assignment_test {
TEST(BasicStringInitializerListAssignTest,
     AssignInitializerListWithinCapacity) {
    easystl::string str(10, 'a');
    str.assign({'H', 'e', 'l', 'l', 'o'});

    EXPECT_EQ(str.size(), 5);
    EXPECT_EQ(str.capacity(), 15);
    EXPECT_EQ(str, "Hello");
}

TEST(BasicStringInitializerListAssignTest,
     AssignInitializerListExceedingCapacity) {
    easystl::string str(5, 'a');
    str.assign({'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'});

    EXPECT_EQ(str.size(), 11);
    EXPECT_GE(str.capacity(), 15);
    EXPECT_EQ(str, "Hello World");
}

TEST(BasicStringInitializerListAssignTest, AssignEmptyInitializerList) {
    easystl::string str("Original");
    str.assign({});

    EXPECT_EQ(str.size(), 0);
    EXPECT_TRUE(str.empty());
}

TEST(BasicStringInitializerListAssignTest,
     AssignInitializerListPreservesAllocator) {
    std::allocator<char> alloc;
    easystl::basic_string<char, easystl::char_traits<char>,
                          std::allocator<char>>
        str("Original", alloc);
    str.assign({'N', 'e', 'w'});

    EXPECT_EQ(str, "New");
    EXPECT_EQ(str.get_allocator(), alloc);
}

TEST(BasicStringInitializerListAssignTest,
     AssignInitializerListLargerThanOriginalCapacity) {
    easystl::string str(3, 'a');
    str.assign({'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'l',
                'o', 'n', 'g', ' ', 's', 't', 'r', 'i', 'n', 'g'});

    EXPECT_EQ(str.size(), 21);
    EXPECT_GE(str.capacity(), 21);
    EXPECT_EQ(str, easystl::string("This is a long string"));
}
} // namespace assignment_test

namespace replacement_test {
// 1. basic_string &replace(size_type pos, size_type n1, const CharType *s,
//                       size_type n2)
// 2. basic_string &replace(size_type pos, size_type n1, const CharType *s)
class BasicStringReplaceWithCStringTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World!"; }
};
TEST_F(BasicStringReplaceWithCStringTest, ReplaceMiddle) {
    str.replace(6, 5, "C++", 3);
    EXPECT_EQ(str, "Hello C++!");
    str.replace(6, 3, "World");
    EXPECT_EQ(str, "Hello World!");
}
TEST_F(BasicStringReplaceWithCStringTest, ReplaceBeginning) {
    str.replace(0, 5, "Goodbye", 7);
    EXPECT_EQ(str, "Goodbye World!");
    str.replace(0, 7, "Hello");
    EXPECT_EQ(str, "Hello World!");
}
TEST_F(BasicStringReplaceWithCStringTest, ReplaceEnd) {
    str.replace(12, 1, "??", 2);
    EXPECT_EQ(str, "Hello World!??");
    str.replace(str.size(), 1, "!!");
    EXPECT_EQ(str, "Hello World!\?\?!!");
}
TEST_F(BasicStringReplaceWithCStringTest, ReplaceWithLongerString) {
    str.replace(6, 5, "Beautiful C++ World", 19);
    EXPECT_EQ(str, "Hello Beautiful C++ World!");
    str.replace(6, 19, "Beautiful C World");
    EXPECT_EQ(str, "Hello Beautiful C World!");
}
TEST_F(BasicStringReplaceWithCStringTest, ReplaceEntireString) {
    str.replace(0, str.length(), "Completely New", 14);
    EXPECT_EQ(str, "Completely New");
}
TEST_F(BasicStringReplaceWithCStringTest, ReplaceWithEmptyString) {
    str.replace(5, 6, "", 0);
    EXPECT_EQ(str, "Hello!");
}
TEST_F(BasicStringReplaceWithCStringTest, ReplaceBeyondStringLength) {
    EXPECT_THROW(str.replace(20, 5, "Test", 4), std::out_of_range);
}
TEST_F(BasicStringReplaceWithCStringTest, ReplaceWithNullTerminatedString) {
    const char *cstr = "C++\0Extra";
    str.replace(6, 5, cstr, 3); // Only replace with "C++"
    EXPECT_EQ(str, "Hello C++!");
    str.replace(6, 5, cstr);
    EXPECT_EQ(str, "Hello C++");
    EXPECT_EQ(str.size(), 9);
}
TEST_F(BasicStringReplaceWithCStringTest, ReplaceWithLongerLengthThanProvided) {
    const char *cstr = "Short";
    str.replace(6, 5, cstr, 10); // Should only use available characters
    EXPECT_EQ(str, "Hello Short!");
}
TEST_F(BasicStringReplaceWithCStringTest, ReplaceWithNullptr) {
    EXPECT_THROW(str.replace(6, 5, nullptr), std::logic_error);
    EXPECT_THROW(str.replace(6, 5, nullptr, 3), std::logic_error);
}
TEST_F(BasicStringReplaceWithCStringTest, ReplaceWithSubstringOfSelf) {
    str.replace(0, str.size(), str.data(), 5);
    EXPECT_EQ(str, "Hello");
    str.replace(0, str.size(), str.data());
    EXPECT_EQ(str, "Hello");
}

// 3. basic_string &replace(size_type pos, size_type n, const basic_string &str)
// 4. basic_string &replace(size_type pos1, size_type n1, const basic_string
// &str, size_type pos2, size_type n2 = npos)
class BasicStringReplaceWithStringTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World!"; }
};
TEST_F(BasicStringReplaceWithStringTest, ReplaceMiddle) {
    str.replace(6, 5, easystl::string("C++"));
    EXPECT_EQ(str, "Hello C++!");
    str.replace(6, 3, easystl::string("Good World"), 5);
    EXPECT_EQ(str, "Hello World!");
}
TEST_F(BasicStringReplaceWithStringTest, ReplaceBeginning) {
    str.replace(0, 5, easystl::string("Goodbye"));
    EXPECT_EQ(str, "Goodbye World!");
    str.replace(0, 7, easystl::string("Hello C++"), 0, 5);
    EXPECT_EQ(str, "Hello World!");
}
TEST_F(BasicStringReplaceWithStringTest, ReplaceEnd) {
    str.replace(str.size(), 1, easystl::string("??"));
    EXPECT_EQ(str, "Hello World!??");
    str.replace(str.size(), 1, easystl::string("!!"), 0, 2);
    EXPECT_EQ(str, "Hello World!\?\?!!");
}
TEST_F(BasicStringReplaceWithStringTest, ReplaceWithLongerString) {
    str.replace(6, 5, easystl::string("Beautiful C++ World"));
    EXPECT_EQ(str, "Hello Beautiful C++ World!");
    str.replace(16, 3, easystl::string("Beautiful C World"), 10, 1);
    EXPECT_EQ(str, "Hello Beautiful C World!");
}
TEST_F(BasicStringReplaceWithStringTest, ReplaceWithEmptyString) {
    str.replace(5, 6, easystl::string());
    EXPECT_EQ(str, "Hello!");
    EXPECT_THROW(str.replace(0, 6, easystl::string(), 34, 23),
                 std::out_of_range);
}
TEST_F(BasicStringReplaceWithStringTest, ReplaceWithNullTerminatedString) {
    easystl::string src("C++\0Extra");
    str.replace(6, 5, src, 0, 3); // Only replace with "C++"
    EXPECT_EQ(str, "Hello C++!");
    str.replace(6, 5, src);
    EXPECT_EQ(str, "Hello C++");
    EXPECT_EQ(str.size(), 9);
    str.replace(0, 5, str, 0, 2);
    EXPECT_EQ(str, "He C++");
}
TEST_F(BasicStringReplaceWithStringTest, ReplaceWithSubstringOfSelf) {
    str.replace(0, str.size(), str);
    EXPECT_EQ(str, "Hello World!");
    str.replace(0, str.size(), str, 0, 5);
    EXPECT_EQ(str, "Hello");
}
// 5. basic_string &replace(size_type pos, size_type n1, size_type n2, CharType
// c)
class BasicStringReplaceWithMultipleCharsTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World!"; }
};
TEST_F(BasicStringReplaceWithMultipleCharsTest, ReplaceWithRepeatedChar) {
    str.replace(6, 5, 3, '+');
    EXPECT_EQ(str, "Hello +++!");
}
TEST_F(BasicStringReplaceWithMultipleCharsTest, ReplaceEntireStringWithChar) {
    str.replace(0, str.length(), str.length(), '-');
    EXPECT_EQ(str, "------------");
}
TEST_F(BasicStringReplaceWithMultipleCharsTest, ReplaceWithMoreChars) {
    str.replace(6, 5, 10, '*');
    EXPECT_EQ(str, "Hello **********!");
}
TEST_F(BasicStringReplaceWithMultipleCharsTest, ReplaceWithFewerChars) {
    str.replace(6, 5, 2, '#');
    EXPECT_EQ(str, "Hello ##!");
}
TEST_F(BasicStringReplaceWithMultipleCharsTest, ReplaceAtBeginning) {
    str.replace(0, 5, 3, 'A');
    EXPECT_EQ(str, "AAA World!");
}
TEST_F(BasicStringReplaceWithMultipleCharsTest, ReplaceAtEnd) {
    str.replace(str.length() - 1, 1, 3, '?');
    EXPECT_EQ(str, "Hello World???");
}
TEST_F(BasicStringReplaceWithMultipleCharsTest, ReplaceWithZeroChars) {
    str.replace(6, 5, 0, 'x');
    EXPECT_EQ(str, "Hello !");
}
TEST_F(BasicStringReplaceWithMultipleCharsTest, ReplaceOutOfRange) {
    EXPECT_THROW(str.replace(str.length() + 1, 1, 1, 'A'), std::out_of_range);
}
TEST_F(BasicStringReplaceWithMultipleCharsTest, ReplaceWithMaxSizeChars) {
    EXPECT_THROW(str.replace(0, str.length(), str.max_size() + 1, 'A'),
                 std::length_error);
}

// 6. basic_string &replace(const_iterator iter1, const_iterator iter2,
//                       const basic_string &str) {
class BasicStringReplaceRangeWithStringTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World!"; }
};
TEST_F(BasicStringReplaceRangeWithStringTest,
       ReplaceEntireStringWithIterators) {
    easystl::string replacement("Goodbye, Universe!");
    str.replace(str.begin(), str.end(), replacement);
    EXPECT_EQ(str, "Goodbye, Universe!");
}
TEST_F(BasicStringReplaceRangeWithStringTest, ReplaceMiddleWithShorterString) {
    easystl::string replacement("New");
    str.replace(str.begin() + 6, str.end() - 1, replacement);
    EXPECT_EQ(str, "Hello New!");
}
TEST_F(BasicStringReplaceRangeWithStringTest, ReplaceMiddleWithLongerString) {
    easystl::string replacement("Wonderful World");
    str.replace(str.begin() + 6, str.end() - 1, replacement);
    EXPECT_EQ(str, "Hello Wonderful World!");
}
TEST_F(BasicStringReplaceRangeWithStringTest, ReplaceWithEmptyString) {
    easystl::string replacement("");
    str.replace(str.begin() + 6, str.end() - 1, replacement);
    EXPECT_EQ(str, "Hello !");
}
TEST_F(BasicStringReplaceRangeWithStringTest, ReplaceEmptyRangeWithString) {
    easystl::string replacement(" Inserted");
    str.replace(str.begin() + 5, str.begin() + 5, replacement);
    EXPECT_EQ(str, "Hello Inserted World!");
}
TEST_F(BasicStringReplaceRangeWithStringTest, ReplaceWithSubstringOfSelf) {
    str.replace(str.begin(), str.end(), str);
    EXPECT_EQ(str, "Hello World!");
}

// 7. basic_string &replace(const_iterator iter1, const_iterator iter2,
//                       const CharType *s, size_type n)
// 8. basic_string &replace(const_iterator iter1, const_iterator iter2,
//                       const CharType *s)
class BasicStringReplaceRangeWithCStringTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World!"; }
};
TEST_F(BasicStringReplaceRangeWithCStringTest, ReplaceEntireStringWithString) {
    const char *replacement = "Goodbye, Universe!";
    str.replace(str.begin(), str.end(), replacement);
    EXPECT_EQ(str, "Goodbye, Universe!");
    str.replace(str.begin(), str.end(), replacement, 7);
    EXPECT_EQ(str, "Goodbye");
}
TEST_F(BasicStringReplaceRangeWithCStringTest, ReplaceMiddleWithShorterString) {
    const char *replacement = "New?";
    str.replace(str.begin() + 6, str.end() - 1, replacement, 3);
    EXPECT_EQ(str, "Hello New!");
    str.replace(str.begin() + 6, str.end(), replacement);
    EXPECT_EQ(str, "Hello New?");
}
TEST_F(BasicStringReplaceRangeWithCStringTest, ReplaceMiddleWithLongerString) {
    easystl::string replacement("Wonderful World");
    str.replace(str.begin() + 6, str.end() - 1, replacement);
    EXPECT_EQ(str, "Hello Wonderful World!");
}
TEST_F(BasicStringReplaceRangeWithCStringTest, ReplaceWithEmptyString) {
    const char *replacement = "";
    str.replace(str.begin() + 6, str.end() - 1, replacement);
    EXPECT_EQ(str, "Hello !");
    str.replace(str.begin() + 6, str.end(), replacement, 10);
    EXPECT_EQ(str, "Hello ");
}
TEST_F(BasicStringReplaceRangeWithCStringTest, ReplaceEmptyRangeWithString) {
    const char *replacement = " Inserted";
    str.replace(str.begin() + 5, str.begin() + 5, replacement);
    EXPECT_EQ(str, "Hello Inserted World!");
}
TEST_F(BasicStringReplaceRangeWithCStringTest, ReplaceWithSubstringOfSelf) {
    str.replace(str.begin(), str.end(), str.data());
    EXPECT_EQ(str, "Hello World!");
    str.replace(str.begin(), str.end(), str.data(), 5);
    EXPECT_EQ(str, "Hello");
}

// 9. basic_string &replace(const_iterator iter1, const_iterator iter2,
//                       size_type n, CharType c)
class BasicStringReplaceRangeWithMultipleCharsTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World!"; }
};
TEST_F(BasicStringReplaceRangeWithMultipleCharsTest, ReplaceWithRepeatedChar) {
    str.replace(str.begin() + 6, str.end() - 1, 3, '+');
    EXPECT_EQ(str, "Hello +++!");
}
TEST_F(BasicStringReplaceRangeWithMultipleCharsTest,
       ReplaceEntireStringWithChar) {
    str.replace(str.begin(), str.end(), str.length(), '-');
    EXPECT_EQ(str, "------------");
}
TEST_F(BasicStringReplaceRangeWithMultipleCharsTest, ReplaceWithMoreChars) {
    str.replace(str.begin() + 6, str.end() - 1, 10, '*');
    EXPECT_EQ(str, "Hello **********!");
}
TEST_F(BasicStringReplaceRangeWithMultipleCharsTest, ReplaceWithFewerChars) {
    str.replace(str.begin() + 6, str.end() - 1, 2, '#');
    EXPECT_EQ(str, "Hello ##!");
}
TEST_F(BasicStringReplaceRangeWithMultipleCharsTest, ReplaceAtBeginning) {
    str.replace(str.begin(), str.begin() + 5, 3, 'A');
    EXPECT_EQ(str, "AAA World!");
}
TEST_F(BasicStringReplaceRangeWithMultipleCharsTest, ReplaceAtEnd) {
    str.replace(str.end() - 1, str.end(), 3, '?');
    EXPECT_EQ(str, "Hello World???");
}
TEST_F(BasicStringReplaceRangeWithMultipleCharsTest, ReplaceWithZeroChars) {
    str.replace(str.begin() + 6, str.end() - 1, 0, 'x');
    EXPECT_EQ(str, "Hello !");
}
TEST_F(BasicStringReplaceRangeWithMultipleCharsTest, ReplaceWithMaxSizeChars) {
    EXPECT_THROW(str.replace(str.begin(), str.end(), str.max_size() + 1, 'A'),
                 std::length_error);
}

// 10. basic_string &replace(const_iterator iter1, const_iterator iter2,
//                       InputIter input_iter1, InputIter input_iter2) {
class BasicStringReplaceRangeWithRangeTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World!"; }
};
TEST_F(BasicStringReplaceRangeWithRangeTest, ReplaceEntireStringWithRange) {
    easystl::string replacement = "Goodbye, Universe!";
    str.replace(str.begin(), str.end(), replacement.begin(), replacement.end());
    EXPECT_EQ(str, "Goodbye, Universe!");
}
TEST_F(BasicStringReplaceRangeWithRangeTest, ReplaceMiddleWithShorterString) {
    easystl::string replacement("New");
    str.replace(str.begin() + 6, str.end() - 1, replacement.begin(),
                replacement.end());
    EXPECT_EQ(str, "Hello New!");
}
TEST_F(BasicStringReplaceRangeWithRangeTest, ReplaceMiddleWithLongerString) {
    easystl::string replacement("Wonderful World");
    str.replace(str.begin(), str.begin() + 6, replacement.begin(),
                replacement.end());
    EXPECT_EQ(str, "Wonderful WorldWorld!");
}
TEST_F(BasicStringReplaceRangeWithRangeTest, ReplaceWithEmptyString) {
    easystl::string replacement("");
    str.replace(str.begin() + 6, str.end() - 1, replacement.begin(),
                replacement.end());
    EXPECT_EQ(str, "Hello !");
}
TEST_F(BasicStringReplaceRangeWithRangeTest, ReplaceEmptyRangeWithString) {
    easystl::string replacement(" Inserted World");
    str.replace(str.begin() + 5, str.begin() + 5, replacement.begin(),
                replacement.begin() + 9);
    EXPECT_EQ(str, "Hello Inserted World!");
}
TEST_F(BasicStringReplaceRangeWithRangeTest, ReplaceWithSubstringOfSelf) {
    str.replace(str.begin(), str.end(), str.begin(), str.begin() + 5);
    EXPECT_EQ(str, "Hello");
}

// 11. basic_string &replace(const_iterator iter1, const_iterator iter2,
//                       std::initializer_list<CharType> l) {
class BasicStringReplaceRangeWithInitializerListTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World!"; }
};
TEST_F(BasicStringReplaceRangeWithInitializerListTest,
       ReplaceEntireStringWithInitializerList) {
    str.replace(
        str.begin(), str.end(),
        std::initializer_list<char>{'G', 'o', 'o', 'd', 'b', 'y', 'e', '!'});
    EXPECT_EQ(str, "Goodbye!");
}
TEST_F(BasicStringReplaceRangeWithInitializerListTest,
       ReplaceMiddleWithShorterString) {
    str.replace(str.begin() + 6, str.end() - 1,
                std::initializer_list<char>{'N', 'e', 'w'});
    EXPECT_EQ(str, "Hello New!");
}
TEST_F(BasicStringReplaceRangeWithInitializerListTest,
       ReplaceMiddleWithLongerString) {
    str.replace(str.begin() + 6, str.end() - 1,
                std::initializer_list<char>{'W', 'o', 'n', 'd', 'e', 'r', 'f',
                                            'u', 'l', ' ', 'W', 'o', 'r', 'l',
                                            'd'});
    EXPECT_EQ(str, "Hello Wonderful World!");
}
TEST_F(BasicStringReplaceRangeWithInitializerListTest,
       ReplaceEmptyRangeWithString) {
    str.replace(str.begin() + 5, str.begin() + 5,
                std::initializer_list<char>{' ', 'I', 'n', 's', 'e', 'r', 't',
                                            'e', 'd'});
    EXPECT_EQ(str, "Hello Inserted World!");
}
} // namespace replacement_test

namespace append_test {
// 1. basic_string &append(const basic_string &str)
class BasicStringAppendStringTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello"; }
};
TEST_F(BasicStringAppendStringTest, AppendShortString) {
    str.append(easystl::string(" World"));
    EXPECT_EQ(str, "Hello World");
}
TEST_F(BasicStringAppendStringTest, AppendLargeString) {
    str.append(easystl::string(1000, 'x'));
    EXPECT_EQ(str.size(), 1005);
    // EXPECT_EQ(str.substr(0, 5), easystl::string("Hello"));
    // EXPECT_EQ(str.substr(5), easystl::string(1000, 'x'));
}
TEST_F(BasicStringAppendStringTest, AppendEmptyString) {
    str.append(easystl::string(""));
    EXPECT_EQ(str, "Hello");
}
TEST_F(BasicStringAppendStringTest, AppendToEmptyString) {
    easystl::string empty("");
    empty.append(str);
    EXPECT_EQ(empty, "Hello");
}
TEST_F(BasicStringAppendStringTest, AppendSelf) {
    str.append(str);
    EXPECT_EQ(str, "HelloHello");
}
TEST_F(BasicStringAppendStringTest, AppendMultipleTimes) {
    str.append(str).append(str);
    EXPECT_EQ(str, "HelloHelloHelloHello");
}

// 2. basic_string &append(const basic_string &str, size_type pos, size_type n =
// npos)
class BasicStringAppendPartStringTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello"; }
};
TEST_F(BasicStringAppendPartStringTest, AppendShortString) {
    str.append(easystl::string(" Beautiful World"), 10, 6);
    EXPECT_EQ(str, "Hello World");
}
TEST_F(BasicStringAppendPartStringTest, AppendLargeString) {
    str.append(easystl::string(1000, 'x'), 500);
    EXPECT_EQ(str.size(), 505);
    // EXPECT_EQ(str.substr(0, 5), easystl::string("Hello"));
    // EXPECT_EQ(str.substr(5), easystl::string(1000, 'x'));
}
TEST_F(BasicStringAppendPartStringTest, AppendEmptyString) {
    str.append(easystl::string(""), 0, 5);
    EXPECT_EQ(str, "Hello");
}
TEST_F(BasicStringAppendPartStringTest, AppendToEmptyString) {
    easystl::string empty("");
    empty.append(str, 3, 2);
    EXPECT_EQ(empty, "lo");
}
TEST_F(BasicStringAppendPartStringTest, AppendSelf) {
    str.append(str, 2);
    EXPECT_EQ(str, "Hellollo");
}
TEST_F(BasicStringAppendPartStringTest, AppendMultipleTimes) {
    str.append(str, 2).append(str, 7, 1);
    EXPECT_EQ(str, "Hellolloo");
}

// 3. basic_string &append(CharType *s, size_type n)
// 4. basic_string &append(CharType *s)
// same as 1 and 2

// 5. basic_string &append(size_type n, CharType c)
class BasicStringAppendMultipleCharsTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello"; }
};
TEST_F(BasicStringAppendMultipleCharsTest, AppendShortChars) {
    str.append(5, 'o');
    EXPECT_EQ(str, "Helloooooo");
}
TEST_F(BasicStringAppendMultipleCharsTest, AppendLargeChars) {
    str.append(1000, 'x');
    EXPECT_EQ(str.size(), 1005);
    // EXPECT_EQ(str.substr(0, 5), easystl::string("Hello"));
    // EXPECT_EQ(str.substr(5), easystl::string(1000, 'x'));
}
TEST_F(BasicStringAppendMultipleCharsTest, AppendToEmptyString) {
    easystl::string empty("");
    empty.append(5, 'h');
    EXPECT_EQ(empty, "hhhhh");
}
TEST_F(BasicStringAppendMultipleCharsTest, AppendMultipleTimes) {
    str.append(2, 'o').append(10, 'h');
    EXPECT_EQ(str, "Hellooohhhhhhhhhh");
}

// basic_string &push_back(CharType c)
class BasicStringAppendOneCharsTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello"; }
};
TEST_F(BasicStringAppendOneCharsTest, PushBackSingleChar) {
    str.push_back('!');
    EXPECT_EQ(str, "Hello!");
}
TEST_F(BasicStringAppendOneCharsTest, PushBackMultipleChars) {
    str.push_back(' ');
    str.push_back('W');
    str.push_back('o');
    str.push_back('r');
    str.push_back('l');
    str.push_back('d');
    EXPECT_EQ(str, "Hello World");
}
TEST_F(BasicStringAppendOneCharsTest, PushBackToEmptyString) {
    easystl::string emptyStr;
    emptyStr.push_back('A');
    EXPECT_EQ(emptyStr, "A");
}
TEST_F(BasicStringAppendOneCharsTest, PushBackNullCharacter) {
    str.push_back('\0');
    EXPECT_EQ(str.size(), 6);
    EXPECT_EQ(str[5], '\0');
}
TEST_F(BasicStringAppendOneCharsTest, PushBackTriggerReallocation) {
    size_t initialCapacity = str.capacity();
    while (str.size() < initialCapacity) {
        str.push_back('a');
    }
    // This should trigger reallocation
    str.push_back('a');
    EXPECT_GT(str.capacity(), initialCapacity);
}
} // namespace append_test

namespace insert_test {
// 1. iterator insert(const_iterator p, size_type n, CharType c)
// 7. basic_string &insert(size_type pos, size_type n, CharType c)
class BasicStringInsertMultipleCharsTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World"; }
};
TEST_F(BasicStringInsertMultipleCharsTest, InsertMiddle) {
    auto it = str.insert(str.begin() + 6, 3, '-');
    EXPECT_EQ(str, "Hello ---World");
    EXPECT_EQ(*it, '-');
    EXPECT_EQ(it - str.begin(), 6);

    EXPECT_EQ(str.insert(6, 3, '-'), "Hello ------World");
}
TEST_F(BasicStringInsertMultipleCharsTest, InsertBeginning) {
    auto it = str.insert(str.begin(), 2, '!');
    EXPECT_EQ(str, "!!Hello World");
    EXPECT_EQ(*it, '!');
    EXPECT_EQ(it, str.begin());

    str.insert(0, 2, '!');
    EXPECT_EQ(str, "!!!!Hello World");
}
TEST_F(BasicStringInsertMultipleCharsTest, InsertEnd) {
    auto it = str.insert(str.end(), 3, '.');
    EXPECT_EQ(str, "Hello World...");
    EXPECT_EQ(*it, '.');
    EXPECT_EQ(it, str.end() - 3);

    str.insert(str.size(), 3, '.');
    EXPECT_EQ(str, "Hello World......");
}
TEST_F(BasicStringInsertMultipleCharsTest, InsertZeroChars) {
    auto it = str.insert(str.begin() + 5, 0, 'x');
    EXPECT_EQ(str, "Hello World");
    EXPECT_EQ(*it, ' ');
    EXPECT_EQ(it - str.begin(), 5);

    str.insert(5, 0, 'x');
    EXPECT_EQ(str, "Hello World");
}
TEST_F(BasicStringInsertMultipleCharsTest, InsertManyChars) {
    auto it = str.insert(str.begin() + 5, 1000, '-');
    EXPECT_EQ(str.size(), 1011);
    // EXPECT_EQ(str.substr(0, 5), "Hello");
    // EXPECT_EQ(str.substr(5, 1000), std::string(1000, '-'));
    // EXPECT_EQ(str.substr(1005), " World");
    EXPECT_EQ(*it, '-');
    EXPECT_EQ(it - str.begin(), 5);

    str.clear();
    str.insert(0, 1000, '-');
    EXPECT_EQ(str.size(), 1000);
}
TEST_F(BasicStringInsertMultipleCharsTest, InsertIntoEmptyString) {
    easystl::string empty_str;
    auto it = empty_str.insert(empty_str.begin(), 5, 'a');
    EXPECT_EQ(empty_str, "aaaaa");
    EXPECT_EQ(*it, 'a');
    EXPECT_EQ(it, empty_str.begin());

    easystl::string another_empty_str;
    another_empty_str.insert(0, 5, 'a');
    EXPECT_EQ(another_empty_str, "aaaaa");
}
TEST_F(BasicStringInsertMultipleCharsTest, InsertNullCharacter) {
    auto it = str.insert(str.begin() + 5, 2, '\0');
    EXPECT_EQ(str.size(), 13);
    // EXPECT_EQ(str.substr(0, 5), "Hello");
    EXPECT_EQ(str[5], '\0');
    EXPECT_EQ(str[6], '\0');
    // EXPECT_EQ(str.substr(7), " World");
    EXPECT_EQ(*it, '\0');

    str.insert(5, 1, '\0');
    EXPECT_EQ(str.size(), 14);
}

// 2. iterator insert(const_iterator p, InputIterator first, InputIterator last)
class BasicStringInsertRangeTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World"; }
};
TEST_F(BasicStringInsertRangeTest, InsertMiddle) {
    easystl::string inserted("Beautiful World");
    auto it =
        str.insert(str.begin() + 6, inserted.begin(), inserted.begin() + 10);
    EXPECT_EQ(str, "Hello Beautiful World");
    EXPECT_EQ(*it, 'B');
    EXPECT_EQ(it - str.begin(), 6);
}
TEST_F(BasicStringInsertRangeTest, InsertBeginning) {
    easystl::string inserted("!!");
    auto it = str.insert(str.begin(), inserted.begin(), inserted.end());
    EXPECT_EQ(str, "!!Hello World");
    EXPECT_EQ(*it, '!');
    EXPECT_EQ(it, str.begin());
}
TEST_F(BasicStringInsertRangeTest, InsertEnd) {
    easystl::string inserted("...");
    auto it = str.insert(str.end(), inserted.begin(), inserted.end());
    EXPECT_EQ(str, "Hello World...");
    EXPECT_EQ(*it, '.');
    EXPECT_EQ(it, str.end() - 3);
}
TEST_F(BasicStringInsertRangeTest, InsertEmptyString) {
    easystl::string insertd("");
    auto it = str.insert(str.begin() + 5, insertd.begin(), insertd.end());
    EXPECT_EQ(str, "Hello World");
    EXPECT_EQ(*it, ' ');
    EXPECT_EQ(it - str.begin(), 5);
}
TEST_F(BasicStringInsertRangeTest, InsertManyChars) {
    easystl::string insertd(1000, '-');
    auto it = str.insert(str.begin() + 5, insertd.begin(), insertd.end());
    EXPECT_EQ(str.size(), 1011);
    // EXPECT_EQ(str.substr(0, 5), "Hello");
    // EXPECT_EQ(str.substr(5, 1000), std::string(1000, '-'));
    // EXPECT_EQ(str.substr(1005), " World");
    EXPECT_EQ(*it, '-');
    EXPECT_EQ(it - str.begin(), 5);
}
TEST_F(BasicStringInsertRangeTest, InsertIntoEmptyString) {
    easystl::string empty_str;
    auto it = empty_str.insert(empty_str.begin(), str.begin(), str.end());
    EXPECT_EQ(empty_str, str);
    EXPECT_EQ(*it, 'H');
    EXPECT_EQ(it, empty_str.begin());
}
TEST_F(BasicStringInsertRangeTest, InsertSelf) {
    auto it = str.insert(str.begin() + 2, str.begin() + 2, str.begin() + 5);
    EXPECT_EQ(str, "Hellollo World");
    EXPECT_EQ(*it, 'l');
    EXPECT_EQ(it, str.begin() + 2);
}

// 3. iterator insert(const_iterator p, std::initializer_list<CharType> l)
class BasicStringInsertInitListTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World"; }
};
TEST_F(BasicStringInsertInitListTest, InsertMiddle) {
    auto it = str.insert(str.begin() + 6,
                         std::initializer_list<char>{'B', 'e', 'a', 'u', 't',
                                                     'i', 'f', 'u', 'l', ' '});
    EXPECT_EQ(str, "Hello Beautiful World");
    EXPECT_EQ(*it, 'B');
    EXPECT_EQ(it - str.begin(), 6);
}
TEST_F(BasicStringInsertInitListTest, InsertBeginning) {
    auto it = str.insert(str.begin(), std::initializer_list<char>{'!', '!'});
    EXPECT_EQ(str, "!!Hello World");
    EXPECT_EQ(*it, '!');
    EXPECT_EQ(it, str.begin());
}
TEST_F(BasicStringInsertInitListTest, InsertEnd) {
    auto it = str.insert(str.end(), std::initializer_list<char>{'.', '.', '.'});
    EXPECT_EQ(str, "Hello World...");
    EXPECT_EQ(*it, '.');
    EXPECT_EQ(it, str.end() - 3);
}
TEST_F(BasicStringInsertInitListTest, InsertEmptyString) {
    EXPECT_THROW(str.insert(str.begin() + 5, std::initializer_list<char>{}),
                 std::logic_error);
}
TEST_F(BasicStringInsertInitListTest, InsertIntoEmptyString) {
    easystl::string empty_str;
    auto it =
        empty_str.insert(empty_str.begin(),
                         std::initializer_list<char>{'H', 'e', 'l', 'l', 'o'});
    EXPECT_EQ(empty_str, "Hello");
    EXPECT_EQ(*it, 'H');
    EXPECT_EQ(it, empty_str.begin());
}

// 4. basic_string &insert(size_type pos, const basic_string &str)
class BasicStringInsertStringTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World"; }
};
TEST_F(BasicStringInsertStringTest, InsertMiddle) {
    easystl::string inserted("Beautiful ");
    str.insert(6, inserted);
    EXPECT_EQ(str, "Hello Beautiful World");
}
TEST_F(BasicStringInsertStringTest, InsertBeginning) {
    easystl::string inserted("!!");
    str.insert(0, inserted);
    EXPECT_EQ(str, "!!Hello World");
}
TEST_F(BasicStringInsertStringTest, InsertEnd) {
    easystl::string inserted("...");
    str.insert(str.size(), inserted);
    EXPECT_EQ(str, "Hello World...");
}
TEST_F(BasicStringInsertStringTest, InsertEmptyString) {
    easystl::string insertd("");
    str.insert(6, insertd);
    EXPECT_EQ(str, "Hello World");
}
TEST_F(BasicStringInsertStringTest, InsertManyChars) {
    easystl::string insertd(1000, '-');
    str.insert(5, insertd);
    EXPECT_EQ(str.size(), 1011);
    // EXPECT_EQ(str.substr(0, 5), "Hello");
    // EXPECT_EQ(str.substr(5, 1000), std::string(1000, '-'));
    // EXPECT_EQ(str.substr(1005), " World");
}
TEST_F(BasicStringInsertStringTest, InsertIntoEmptyString) {
    easystl::string empty_str;
    empty_str.insert(0, str);
    EXPECT_EQ(empty_str, str);
}
TEST_F(BasicStringInsertStringTest, InsertSelf) {
    str.insert(2, str);
    EXPECT_EQ(str, "HeHello Worldllo World");
}

// 5. basic_string &insert(size_type pos1, const basic_string &str,
//                      size_type pos2, size_type n = npos)
class BasicStringInsertSubstringTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World"; }
};
TEST_F(BasicStringInsertSubstringTest, InsertMiddle) {
    easystl::string inserted("Beautiful World");
    str.insert(6, inserted, 0, 10);
    EXPECT_EQ(str, "Hello Beautiful World");
}
TEST_F(BasicStringInsertSubstringTest, InsertBeginning) {
    easystl::string inserted("!!");
    str.insert(0, inserted, 0, 2);
    EXPECT_EQ(str, "!!Hello World");
}
TEST_F(BasicStringInsertSubstringTest, InsertEnd) {
    easystl::string inserted("...");
    str.insert(str.size(), inserted, 0);
    EXPECT_EQ(str, "Hello World...");
}
TEST_F(BasicStringInsertSubstringTest, InsertEmptyString) {
    easystl::string insertd("");
    str.insert(6, insertd, 0, 10);
    EXPECT_EQ(str, "Hello World");
}
TEST_F(BasicStringInsertSubstringTest, InsertManyChars) {
    easystl::string insertd(1000, '-');
    str.insert(5, insertd, 500, 200);
    EXPECT_EQ(str.size(), 211);
    // EXPECT_EQ(str.substr(0, 5), "Hello");
    // EXPECT_EQ(str.substr(5, 1000), std::string(1000, '-'));
    // EXPECT_EQ(str.substr(1005), " World");
}
TEST_F(BasicStringInsertSubstringTest, InsertIntoEmptyString) {
    easystl::string empty_str;
    empty_str.insert(0, str, 6, 2);
    EXPECT_EQ(empty_str, "Wo");
}
TEST_F(BasicStringInsertSubstringTest, Insertself) {
    str.insert(2, str, 2, 3);
    EXPECT_EQ(str, "Hellollo World");
}

// 6. basic_string &insert(size_type pos1, const CharType *s, size_type n)
class BasicStringInsertCSubstringTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World"; }
};
TEST_F(BasicStringInsertCSubstringTest, InsertMiddle) {
    str.insert(6, "Beautiful World", 10);
    EXPECT_EQ(str, "Hello Beautiful World");
}
TEST_F(BasicStringInsertCSubstringTest, InsertBeginning) {
    str.insert(0, "!!", 2);
    EXPECT_EQ(str, "!!Hello World");
}
TEST_F(BasicStringInsertCSubstringTest, InsertEnd) {
    str.insert(str.size(), "...", 3);
    EXPECT_EQ(str, "Hello World...");
}
TEST_F(BasicStringInsertCSubstringTest, InsertEmptyString) {
    str.insert(6, "", 10);
    EXPECT_EQ(str, "Hello World");
}
TEST_F(BasicStringInsertCSubstringTest, InsertManyChars) {
    easystl::string insertd(1000, '-');
    str.insert(5, insertd.data(), 200);
    EXPECT_EQ(str.size(), 211);
    // EXPECT_EQ(str.substr(0, 5), "Hello");
    // EXPECT_EQ(str.substr(5, 1000), std::string(1000, '-'));
    // EXPECT_EQ(str.substr(1005), " World");
}
TEST_F(BasicStringInsertCSubstringTest, InsertIntoEmptyString) {
    easystl::string empty_str;
    empty_str.insert(0, str.data(), 2);
    EXPECT_EQ(empty_str, "He");
}
TEST_F(BasicStringInsertCSubstringTest, Insertself) {
    str.insert(2, str.data(), 3);
    EXPECT_EQ(str, "HeHelllo World");
}

// 6. basic_string &insert(size_type pos1, const CharType *s)
class BasicStringInsertCStringTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World"; }
};
TEST_F(BasicStringInsertCStringTest, InsertMiddle) {
    str.insert(6, "Beautiful ");
    EXPECT_EQ(str, "Hello Beautiful World");
}
TEST_F(BasicStringInsertCStringTest, InsertBeginning) {
    str.insert(0, "!!");
    EXPECT_EQ(str, "!!Hello World");
}
TEST_F(BasicStringInsertCStringTest, InsertEnd) {
    str.insert(str.size(), "...");
    EXPECT_EQ(str, "Hello World...");
}
TEST_F(BasicStringInsertCStringTest, InsertEmptyString) {
    str.insert(6, "");
    EXPECT_EQ(str, "Hello World");
}
TEST_F(BasicStringInsertCStringTest, InsertManyChars) {
    easystl::string insertd(1000, '-');
    str.insert(5, insertd.data());
    EXPECT_EQ(str.size(), 1011);
    // EXPECT_EQ(str.substr(0, 5), "Hello");
    // EXPECT_EQ(str.substr(5, 1000), std::string(1000, '-'));
    // EXPECT_EQ(str.substr(1005), " World");
}
TEST_F(BasicStringInsertCStringTest, InsertIntoEmptyString) {
    easystl::string empty_str;
    empty_str.insert(0, str.data());
    EXPECT_EQ(empty_str, str);
}
TEST_F(BasicStringInsertCStringTest, Insertself) {
    str.insert(2, str.data());
    EXPECT_EQ(str, "HeHello Worldllo World");
}

// 8. iterator insert(const_iterator p, CharType c)
class BasicStringInsertOneCharTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello World"; }
};
TEST_F(BasicStringInsertOneCharTest, InsertMiddle) {
    str.insert(str.begin() + 5, ',');
    EXPECT_EQ(str, "Hello, World");
}
TEST_F(BasicStringInsertOneCharTest, InsertBeginning) {
    str.insert(str.begin(), '!');
    EXPECT_EQ(str, "!Hello World");
}
TEST_F(BasicStringInsertOneCharTest, InsertEnd) {
    str.insert(str.end(), '!');
    EXPECT_EQ(str, "Hello World!");
}
TEST_F(BasicStringInsertOneCharTest, InsertIntoEmptyString) {
    easystl::string empty_str;
    empty_str.insert(empty_str.begin(), 'H');
    EXPECT_EQ(empty_str, "H");
}
} // namespace insert_test

namespace erase_test {
// 1. basic_string &erase(size_type pos, size_type n = npos)
class BasicStringEraseMultipleCharsTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello, World!"; }
};
TEST_F(BasicStringEraseMultipleCharsTest, EraseMiddle) {
    str.erase(5, 2);
    EXPECT_EQ(str, "HelloWorld!");
}
TEST_F(BasicStringEraseMultipleCharsTest, EraseBeginning) {
    str.erase(0, 7);
    EXPECT_EQ(str, "World!");
}
TEST_F(BasicStringEraseMultipleCharsTest, EraseEnd) {
    str.erase(5);
    EXPECT_EQ(str, "Hello");
}
TEST_F(BasicStringEraseMultipleCharsTest, EraseAll) {
    str.erase(0);
    EXPECT_TRUE(str.empty());
}
TEST_F(BasicStringEraseMultipleCharsTest, EraseNone) {
    str.erase(0, 0);
    EXPECT_EQ(str, "Hello, World!");
}
TEST_F(BasicStringEraseMultipleCharsTest, EraseOutOfRange) {
    EXPECT_THROW(str.erase(str.size() + 1), std::out_of_range);
}
TEST_F(BasicStringEraseMultipleCharsTest, EraseBeyondEnd) {
    str.erase(5, 100); // Should truncate at end of string
    EXPECT_EQ(str, "Hello");
}
TEST_F(BasicStringEraseMultipleCharsTest, EraseWithNpos) {
    str.erase(7, easystl::string::npos);
    EXPECT_EQ(str, "Hello, ");
}
TEST_F(BasicStringEraseMultipleCharsTest, EraseMiddleMultipleTimes) {
    str.erase(5, 1).erase(5, 1);
    EXPECT_EQ(str, "HelloWorld!");
}
TEST_F(BasicStringEraseMultipleCharsTest, EraseFromEmptyString) {
    easystl::string empty_str;
    EXPECT_NO_THROW(empty_str.erase(0));
    EXPECT_TRUE(empty_str.empty());
}

// 2. iterator erase(const_iterator it)
class BasicStringEraseOneCharTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello, World!"; }
};
TEST_F(BasicStringEraseOneCharTest, EraseMiddle) {
    str.erase(str.begin() + 5);
    EXPECT_EQ(str, "Hello World!");
}
TEST_F(BasicStringEraseOneCharTest, EraseBeginning) {
    str.erase(str.begin() + 0);
    EXPECT_EQ(str, "ello, World!");
}
TEST_F(BasicStringEraseOneCharTest, EraseEnd) {
    str.erase(str.end() - 1);
    EXPECT_EQ(str, "Hello, World");
}
TEST_F(BasicStringEraseOneCharTest, PopBack) {
    str.pop_back();
    EXPECT_EQ(str, "Hello, World");
}

// 3. iterator erase(const_iterator first, const_iterator last)
class BasicStringEraseRangeCharsTest : public ::testing::Test {
  protected:
    easystl::string str;
    void SetUp() override { str = "Hello, World!"; }
};
TEST_F(BasicStringEraseRangeCharsTest, EraseMiddle) {
    str.erase(str.begin() + 5, str.begin() + 7);
    EXPECT_EQ(str, "HelloWorld!");
}
TEST_F(BasicStringEraseRangeCharsTest, EraseBeginning) {
    str.erase(str.begin(), str.begin() + 7);
    EXPECT_EQ(str, "World!");
}
TEST_F(BasicStringEraseRangeCharsTest, EraseEnd) {
    str.erase(str.begin() + 5, str.end());
    EXPECT_EQ(str, "Hello");
}
TEST_F(BasicStringEraseRangeCharsTest, EraseAll) {
    str.erase(str.begin(), str.end());
    EXPECT_TRUE(str.empty());
}
TEST_F(BasicStringEraseRangeCharsTest, EraseNone) {
    str.erase(str.begin() + 2, str.begin() + 2);
    EXPECT_EQ(str, "Hello, World!");
}
TEST_F(BasicStringEraseRangeCharsTest, EraseFromEmptyString) {
    easystl::string empty_str;
    empty_str.erase(empty_str.begin(), empty_str.end());
    EXPECT_TRUE(empty_str.empty());
}
} // namespace erase_test

namespace swap_test {
TEST(BasicStringSwapTest, SwapEmptyStrings) {
    easystl::string s1;
    easystl::string s2;
    s1.swap(s2);
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s2.empty());
}
TEST(BasicStringSwapTest, SwapSmallStrings) {
    easystl::string s1("hello");
    easystl::string s2("world");
    s1.swap(s2);
    EXPECT_EQ(s1, "world");
    EXPECT_EQ(s2, "hello");
    EXPECT_EQ(s1.capacity(), 15);
}
TEST(BasicStringSwapTest, SmallStringSwapEmptyString) {
    easystl::string s1("hello");
    easystl::string s2;
    s1.swap(s2);
    EXPECT_TRUE(s1.empty());
    EXPECT_EQ(s2, "hello");
    EXPECT_EQ(s2.capacity(), 15);
    EXPECT_EQ(s1.length(), 0);
}
TEST(BasicStringSwapTest, EmptyStringSwapSmallString) {
    easystl::string s1("hello");
    easystl::string s2;
    s2.swap(s1);
    EXPECT_TRUE(s1.empty());
    EXPECT_EQ(s2, "hello");
    EXPECT_EQ(s2.capacity(), 15);
    EXPECT_EQ(s1.length(), 0);
}
TEST(BasicStringSwapTest, StringSwapSmallString) {
    easystl::string s1(100, 'a');
    easystl::string s2(10, 'b');
    s1.swap(s2);
    EXPECT_EQ(s1.capacity(), 15);
    EXPECT_EQ(s2.capacity(), 100);
}
TEST(BasicStringSwapTest, SmallStringSwapString) {
    easystl::string s1(100, 'a');
    easystl::string s2(10, 'b');
    s2.swap(s1);
    EXPECT_EQ(s1.capacity(), 15);
    EXPECT_EQ(s2.capacity(), 100);
}
TEST(BasicStringSwapTest, StringSwapString) {
    easystl::string s1(200, 'a');
    easystl::string s2(100, 'b');
    s1.swap(s2);
    EXPECT_EQ(s1.capacity(), 100);
    EXPECT_EQ(s2.capacity(), 200);
}
TEST(BasicStringSwapTest, SwapSelfDoesNothing) {
    easystl::string s1("Hello");
    s1.swap(s1);
    EXPECT_EQ(s1, "Hello");
}
} // namespace swap_test

namespace copy_test {
class BasicStringCopyTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World!"; }
};

TEST_F(BasicStringCopyTest, CopyEntireString) {
    char buffer[20] = {0};
    size_t copied = str.copy(buffer, str.length(), 0);

    EXPECT_EQ(copied, str.length());
    EXPECT_STREQ(buffer, str.c_str());
}
TEST_F(BasicStringCopyTest, CopyPartialString) {
    char buffer[10] = {0};
    size_t copied = str.copy(buffer, 5, 7);

    EXPECT_EQ(copied, 5);
    EXPECT_STREQ(buffer, "World");
}

TEST_F(BasicStringCopyTest, CopyWithPosOutOfRange) {
    char buffer[10] = {0};
    EXPECT_THROW(str.copy(buffer, 5, str.length() + 1), std::out_of_range);
}

TEST_F(BasicStringCopyTest, CopyZeroCharacters) {
    char buffer[10] = {0};
    size_t copied = str.copy(buffer, 0, 0);

    EXPECT_EQ(copied, 0);
    EXPECT_STREQ(buffer, "");
}

TEST_F(BasicStringCopyTest, CopyWithLargeN) {
    char buffer[20] = {0};
    size_t copied = str.copy(buffer, 100, 0);

    EXPECT_EQ(copied, str.length());
    EXPECT_STREQ(buffer, str.c_str());
}
} // namespace copy_test

namespace find_test {
// 1. size_type find(const CharType *s, size_type pos, size_type n)
// 3. size_type find(const CharType *s, size_type pos = 0)
class BasicStringFindCStringTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World! Hello, C++!"; }
};
TEST_F(BasicStringFindCStringTest, FindFromStart) {
    EXPECT_EQ(str.find("World", 0, 5), 7);
    EXPECT_EQ(str.find("Hello", 0, 5), 0);
    EXPECT_EQ(str.find("C++", 0, 3), 21);

    EXPECT_EQ(str.find("World"), 7);
    EXPECT_EQ(str.find("Hello"), 0);
    EXPECT_EQ(str.find("C++"), 21);
}
TEST_F(BasicStringFindCStringTest, FindFromMiddle) {
    EXPECT_EQ(str.find("Hello", 13, 3), 14);
    EXPECT_EQ(str.find("World", 13, 3), easystl::string::npos);

    EXPECT_EQ(str.find("Hello", 13), 14);
    EXPECT_EQ(str.find("World", 13), easystl::string::npos);
}
TEST_F(BasicStringFindCStringTest, FindNonExistentSubstring) {
    EXPECT_EQ(str.find("Python", 0, 6), easystl::string::npos);
    EXPECT_EQ(str.find("Python"), easystl::string::npos);
}
TEST_F(BasicStringFindCStringTest, FindEmptySubstring) {
    EXPECT_EQ(str.find("", 0, 4), easystl::string::npos);
    EXPECT_EQ(str.find("", 5, 8), easystl::string::npos);
    EXPECT_EQ(str.find("", str.size(), 4), easystl::string::npos);

    EXPECT_EQ(str.find("", 0), 0);
    EXPECT_EQ(str.find("", 5), 5);
    EXPECT_EQ(str.find("", str.size()), str.size());
}
TEST_F(BasicStringFindCStringTest, FindSingleCharacter) {
    EXPECT_EQ(str.find("W", 0, 1), 7);
    EXPECT_EQ(str.find("!", 0, 1), 12);
    EXPECT_EQ(str.find("!", 13, 1), 24);

    EXPECT_EQ(str.find("W"), 7);
    EXPECT_EQ(str.find("!"), 12);
    EXPECT_EQ(str.find("!", 13), 24);
}

// 2. size_type find(const basic_string &str, size_type pos = 0)
class BasicStringFindStringTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World! Hello, C++!"; }
};
TEST_F(BasicStringFindStringTest, FindFromStart) {
    EXPECT_EQ(str.find(easystl::string("World")), 7);
    EXPECT_EQ(str.find(easystl::string("Hello")), 0);
    EXPECT_EQ(str.find(easystl::string("C++")), 21);
}
TEST_F(BasicStringFindStringTest, FindFromMiddle) {
    EXPECT_EQ(str.find(easystl::string("Hello"), 10), 14);
    EXPECT_EQ(str.find(easystl::string("World"), 8), easystl::string::npos);
}
TEST_F(BasicStringFindStringTest, FindNonExistentSubstring) {
    EXPECT_EQ(str.find(easystl::string("Python"), 0), easystl::string::npos);
}
TEST_F(BasicStringFindStringTest, FindEmptySubstring) {
    EXPECT_EQ(str.find(easystl::string(""), 5), 5);
}
TEST_F(BasicStringFindStringTest, FindWithLengthParameter) {
    EXPECT_EQ(str.find(easystl::string("Hello, World!"), 0), 0);
    EXPECT_EQ(str.find(easystl::string("Hello, World!"), 14),
              easystl::string::npos);
}
TEST_F(BasicStringFindStringTest, FindSingleCharacter) {
    EXPECT_EQ(str.find(easystl::string("W"), 0), 7);
    EXPECT_EQ(str.find(easystl::string("!"), 0), 12);
    EXPECT_EQ(str.find(easystl::string("!"), 13), 24);
}

// 4. size_type find(const CharType c, size_type pos = 0)
class BasicStringFindCharTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World! Hello, C++!"; }
};
TEST_F(BasicStringFindCharTest, FindSingleCharacter) {
    EXPECT_EQ(str.find('H'), 0);
    EXPECT_EQ(str.find('o'), 4);
    EXPECT_EQ(str.find('!'), 12);
    EXPECT_EQ(str.find(','), 5);
}
TEST_F(BasicStringFindCharTest, FindSingleCharacterWithStartPos) {
    EXPECT_EQ(str.find('H', 1), 14);
    EXPECT_EQ(str.find('o', 5), 8);
    EXPECT_EQ(str.find('!', 13), 24);
}
TEST_F(BasicStringFindCharTest, FindNonExistentCharacter) {
    EXPECT_EQ(str.find('z', 0), easystl::string::npos);
}
TEST_F(BasicStringFindCharTest, FindCharacterAtEndOfString) {
    EXPECT_EQ(str.find('!', 24), 24);
}
TEST_F(BasicStringFindCharTest, FindCharacterWithOutOfRangePos) {
    EXPECT_EQ(str.find('H', str.size()), easystl::string::npos);
    EXPECT_EQ(str.find('H', str.size() + 1), easystl::string::npos);
}
TEST_F(BasicStringFindCharTest, FindCharacterInEmptyString) {
    easystl::string empty_string;
    EXPECT_EQ(empty_string.find('a', 0), easystl::string::npos);
}
TEST_F(BasicStringFindCharTest, FindNullCharacter) {
    easystl::string string_with_null("Hello\0World", 11);
    EXPECT_EQ(string_with_null.find('\0', 0), 5);
}
} // namespace find_test

namespace rfind_test {
// 1. size_type rfind(const CharType *s, size_type pos, size_type n)
// 3. size_type rfind(const CharType *s, size_type pos = npos)
class BasicStringRFindCStringTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World! Hello, C++!"; }
};
TEST_F(BasicStringRFindCStringTest, RFindFromEnd) {
    EXPECT_EQ(str.rfind("World", str.size(), 5), 7);
    EXPECT_EQ(str.rfind("C++", str.size(), 3), 21);
    EXPECT_EQ(str.rfind("Hello", str.size(), 5), 14);

    EXPECT_EQ(str.rfind("World"), 7);
    EXPECT_EQ(str.rfind("Hello"), 14);
    EXPECT_EQ(str.rfind("C++"), 21);
}
TEST_F(BasicStringRFindCStringTest, RFindFromMiddle) {
    EXPECT_EQ(str.rfind("C++", 14, 3), easystl::string::npos);
    EXPECT_EQ(str.rfind("Hello", 10, 5), 0);

    EXPECT_EQ(str.rfind("Hello", 13), 0);
    EXPECT_EQ(str.rfind("World", 13), 7);
}
TEST_F(BasicStringRFindCStringTest, RFindNonExistentSubstring) {
    EXPECT_EQ(str.rfind("Python", str.size(), 6), easystl::string::npos);
    EXPECT_EQ(str.rfind("Python"), easystl::string::npos);
}
TEST_F(BasicStringRFindCStringTest, RFindEmptySubstring) {
    EXPECT_EQ(str.find("", 0, 4), easystl::string::npos);
    EXPECT_EQ(str.find("", 5, 8), easystl::string::npos);
    EXPECT_EQ(str.find("", str.size(), 4), easystl::string::npos);

    EXPECT_EQ(str.rfind("", 0), 0);
    EXPECT_EQ(str.rfind("", 5), 5);
    EXPECT_EQ(str.rfind(""), str.size());
}
TEST_F(BasicStringRFindCStringTest, RFindSingleCharacter) {
    EXPECT_EQ(str.rfind("W", str.size(), 1), 7);
    EXPECT_EQ(str.rfind("!", str.size(), 1), 24);
    EXPECT_EQ(str.rfind("!", 14, 1), 12);

    EXPECT_EQ(str.rfind("W"), 7);
    EXPECT_EQ(str.rfind("!"), 24);
    EXPECT_EQ(str.rfind("!", 13), 12);
}

// 2. size_type rfind(const basic_string &str, size_type pos = 0)
class BasicStringRFindStringTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World! Hello, C++!"; }
};
TEST_F(BasicStringRFindStringTest, FindFromEnd) {
    EXPECT_EQ(str.rfind(easystl::string("World")), 7);
    EXPECT_EQ(str.rfind(easystl::string("Hello")), 14);
    EXPECT_EQ(str.rfind(easystl::string("C++")), 21);
}
TEST_F(BasicStringRFindStringTest, FindFromMiddle) {
    EXPECT_EQ(str.rfind(easystl::string("Hello"), 13), 0);
    EXPECT_EQ(str.rfind(easystl::string("World"), 13), 7);
    EXPECT_EQ(str.rfind(easystl::string("C++", 13)), easystl::string::npos);
}
TEST_F(BasicStringRFindStringTest, FindNonExistentSubstring) {
    EXPECT_EQ(str.rfind(easystl::string("Python")), easystl::string::npos);
}
TEST_F(BasicStringRFindStringTest, FindEmptySubstring) {
    EXPECT_EQ(str.rfind(easystl::string("")), str.size());
    EXPECT_EQ(str.rfind(easystl::string(""), 0), 0);
    EXPECT_EQ(str.rfind(easystl::string(""), 13), 13);
}
TEST_F(BasicStringRFindStringTest, FindSingleCharacter) {
    EXPECT_EQ(str.rfind(easystl::string("W"), str.size()), 7);
    EXPECT_EQ(str.rfind(easystl::string("!"), str.size()), 24);
    EXPECT_EQ(str.rfind(easystl::string("!"), 13), 12);
}

// 4. size_type rfind(const CharType c, size_type pos = npos)
class BasicStringRFindCharTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World! Hello, C++!"; }
};
TEST_F(BasicStringRFindCharTest, FindSingleCharacter) {
    EXPECT_EQ(str.rfind('H'), 14);
    EXPECT_EQ(str.rfind('o'), 18);
    EXPECT_EQ(str.rfind('!'), 24);
    EXPECT_EQ(str.rfind(','), 19);
}
TEST_F(BasicStringRFindCharTest, FindSingleCharacterWithStartPos) {
    EXPECT_EQ(str.rfind('H', 13), 0);
    EXPECT_EQ(str.rfind('o', 5), 4);
    EXPECT_EQ(str.rfind('!', 13), 12);
}
TEST_F(BasicStringRFindCharTest, FindNonExistentCharacter) {
    EXPECT_EQ(str.rfind('z'), easystl::string::npos);
}
TEST_F(BasicStringRFindCharTest, FindCharacterInEmptyString) {
    easystl::string empty_string;
    EXPECT_EQ(empty_string.rfind('a', 0), easystl::string::npos);
}
TEST_F(BasicStringRFindCharTest, FindNullCharacter) {
    easystl::string string_with_null("Hello\0World", 11);
    EXPECT_EQ(string_with_null.rfind('\0'), 5);
}
} // namespace rfind_test

namespace find_first_of_test {
class BasicStringFindFirstOfTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World! Hello, C++!"; }
};
TEST_F(BasicStringFindFirstOfTest, FindFirstOfFromStart) {
    EXPECT_EQ(str.find_first_of("aeiou", 0, 3), 1); // 'e' in "Hello"
    EXPECT_EQ(str.find_first_of("xyz", 0, 3), easystl::string::npos);
}
TEST_F(BasicStringFindFirstOfTest, FindFirstOfSingleChar) {
    EXPECT_EQ(str.find_first_of("H", 0, 1), 0);
    EXPECT_EQ(str.find_first_of("H", 6, 1), 14); // Second "Hello"
}

TEST_F(BasicStringFindFirstOfTest, FindFirstOfMultipleMatches) {
    EXPECT_EQ(str.find_first_of("ol", 0, 2), 2); // 'l' in "Hello"
    EXPECT_EQ(str.find_first_of("ol", 3, 2), 3); // 'o' in "Hello"
}

TEST_F(BasicStringFindFirstOfTest, FindFirstOfNoMatch) {
    EXPECT_EQ(str.find_first_of("xyz", 0, 3), easystl::string::npos);
}

TEST_F(BasicStringFindFirstOfTest, FindFirstOfEmptySearchString) {
    EXPECT_EQ(str.find_first_of("", 0), easystl::string::npos);
}

TEST_F(BasicStringFindFirstOfTest, FindFirstOfWithPosOutOfRange) {
    EXPECT_EQ(str.find_first_of("Hello", str.size()), easystl::string::npos);
    EXPECT_EQ(str.find_first_of("Hello", str.size() + 1),
              easystl::string::npos);
}

TEST_F(BasicStringFindFirstOfTest, FindFirstOfInEmptyString) {
    easystl::string empty_string;
    EXPECT_EQ(empty_string.find_first_of("abc", 0), easystl::string::npos);
}

TEST_F(BasicStringFindFirstOfTest, FindFirstOfWithNullCharacter) {
    easystl::string string_with_null("Hello\0World", 11);
    EXPECT_EQ(string_with_null.find_first_of("d\0", 0, 2),
              5); // Finds the null character
}
} // namespace find_first_of_test

namespace find_last_of_test {
class BasicStringFindLastOfTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World! Hello, C++!"; }
};
TEST_F(BasicStringFindLastOfTest, FindLastOfFromStart) {
    EXPECT_EQ(str.find_last_of("aeiou", str.size(), 5),
              18); // 'o' in second "Hello"
    EXPECT_EQ(str.find_last_of("xyz", str.size(), 3), easystl::string::npos);
}
TEST_F(BasicStringFindLastOfTest, FindLastOfSingleChar) {
    EXPECT_EQ(str.find_last_of("H", str.size(), 1), 14);
    EXPECT_EQ(str.find_last_of("H", 6, 1), 0); // first "Hello"
}

TEST_F(BasicStringFindLastOfTest, FindLastOfMultipleMatches) {
    EXPECT_EQ(str.find_last_of("ol", str.size(), 2), 18);     // 'l' in "Hello"
    EXPECT_EQ(str.find_last_of("ol", str.size() / 2, 2), 10); // 'o' in "World"
}

TEST_F(BasicStringFindLastOfTest, FindLastOfNoMatch) {
    EXPECT_EQ(str.find_last_of("xyz", 0, 3), easystl::string::npos);
}

TEST_F(BasicStringFindLastOfTest, FindLastOfEmptySearchString) {
    EXPECT_EQ(str.find_last_of("", 0), easystl::string::npos);
}
TEST_F(BasicStringFindLastOfTest, FindLastOfInEmptyString) {
    easystl::string empty_string;
    EXPECT_EQ(empty_string.find_last_of("abc", 0, 3), easystl::string::npos);
}
TEST_F(BasicStringFindLastOfTest, FindLastOfWithNullCharacter) {
    easystl::string string_with_null("Hello\0World\0", 12);
    EXPECT_EQ(string_with_null.find_last_of("\0d", str.size(), 2),
              11); // Finds the null character
}
} // namespace find_last_of_test

namespace find_first_not_of_test {
TEST(BasicStringFindFirstNotOfTest, FindFirstNotOf) {
    const char *pattern = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "abcdefghijklmnopqrstuvwxyz"
                          "0123456789";

    easystl::string data = "1) %FIX, 2) %HACK, 3) %TODO";
    const easystl::string replacement = "%DONE%";

    for (easystl::string::size_type first{}, last{};
         (first = data.find('%', first)) != easystl::string::npos;
         first += replacement.size()) {
        last = data.find_first_not_of(pattern, first + 1);
        if (last == easystl::string::npos)
            last = data.length();

        // 现在 first 位于 '%'，而 last 位于找到的子串的尾后位置
        data.replace(first, last - first, replacement);
    }

    EXPECT_EQ(data, "1) %DONE%, 2) %DONE%, 3) %DONE%");
}
TEST(BasicStringFindFirstNotOfTest, FindFirstNotOfSingleChar) {
    const easystl::string str("Hello World!");

    EXPECT_EQ(str.find_first_not_of('a'), 0);
    EXPECT_EQ(str.find_first_not_of('l', 2), 4);
}
} // namespace find_first_not_of_test

namespace find_last_not_of_test {
TEST(BasicStringFindLastNotOfTest, FindLastNotOfCString) {
    easystl::string str{"Hello World World"};
    EXPECT_EQ(str.find_last_not_of("dlr", str.size(), 3), 13);
    EXPECT_EQ(str.find_last_not_of("dlr", str.size() / 2, 3), 7);
    EXPECT_EQ(str.find_last_not_of("Hello World", str.size(), 11),
              easystl::string::npos);
    EXPECT_EQ(str.find_last_not_of("o", str.size() + 2, 11), 16);
}
TEST(BasicStringFindLastNotOfTest, FindLastNotOfSingleChar) {
    easystl::string str{"aaabaaa"};
    EXPECT_EQ(str.find_last_not_of('a'), 3);
}
} // namespace find_last_not_of_test

namespace substr_test {
class BasicStringSubstrTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World! Hello, C++!"; }
};
TEST_F(BasicStringSubstrTest, SubstrEntireString) {
    EXPECT_EQ(str.substr(), "Hello, World! Hello, C++!");
}
TEST_F(BasicStringSubstrTest, SubstrFromPosition) {
    EXPECT_EQ(str.substr(7), "World! Hello, C++!");
}
TEST_F(BasicStringSubstrTest, SubstrWithLength) {
    EXPECT_EQ(str.substr(0, 5), "Hello");
    EXPECT_EQ(str.substr(7, 5), "World");
}
TEST_F(BasicStringSubstrTest, SubstrEmptyResult) {
    EXPECT_EQ(str.substr(str.size()), "");
}
TEST_F(BasicStringSubstrTest, SubstrWithExcessiveLength) {
    EXPECT_EQ(str.substr(0, 100), "Hello, World! Hello, C++!");
}
TEST_F(BasicStringSubstrTest, SubstrFromEmptyString) {
    easystl::string empty_string;
    EXPECT_EQ(empty_string.substr(), "");
    EXPECT_EQ(empty_string.substr(0, 10), "");
}
TEST_F(BasicStringSubstrTest, SubstrWithNpos) {
    EXPECT_EQ(str.substr(0, easystl::string::npos),
              "Hello, World! Hello, C++!");
    EXPECT_EQ(str.substr(7, easystl::string::npos), "World! Hello, C++!");
}
TEST_F(BasicStringSubstrTest, SubstrInvalidPos) {
    EXPECT_THROW(str.substr(str.size() + 1), std::out_of_range);
}
TEST_F(BasicStringSubstrTest, SubstrWithNullCharacter) {
    easystl::string string_with_null("Hello\0World", 11);
    EXPECT_EQ(string_with_null.substr(5, 6), easystl::string("\0World", 6));
}
} // namespace substr_test

namespace compare_test {

// 1. int compare(basic_string &str) const
class BasicStringStrCompareStrTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World!"; }
};
TEST_F(BasicStringStrCompareStrTest, CompareEqual) {
    easystl::string str1("Hello, World!");
    EXPECT_EQ(str.compare(str1), 0);
}
TEST_F(BasicStringStrCompareStrTest, CompareLessThan) {
    // not equal length
    easystl::string str1("Hello, World! Extra");
    EXPECT_LT(str.compare(str1), 0);

    easystl::string str2("Hello, Worle! Extra");
    EXPECT_LT(str.compare(str2), 0);

    // equal length
    easystl::string str3("Hello, Worlg!");
    EXPECT_LT(str.compare(str3), 0);
}
TEST_F(BasicStringStrCompareStrTest, CompareGreaterThan) {
    // not equal length
    easystl::string str1("Hello, Worl");
    EXPECT_GT(str.compare(str1), 0);

    easystl::string str2("Helln, World! Extra");
    EXPECT_GT(str.compare(str2), 0);

    // equal length
    easystl::string str3("Hello, Worlc!");
    EXPECT_GT(str.compare(str3), 0);
}
TEST_F(BasicStringStrCompareStrTest, CompareEmptyString) {
    easystl::string empty_str;
    EXPECT_GT(str.compare(empty_str), 0);
    EXPECT_LT(empty_str.compare(str), 0);
}
TEST_F(BasicStringStrCompareStrTest, CompareSameObject) {
    EXPECT_EQ(str.compare(str), 0);
}
TEST_F(BasicStringStrCompareStrTest, CompareWithNullCharacter) {
    easystl::string str1("Hello\0World", 11);
    easystl::string str2("Hello\0Venus", 11);
    EXPECT_GT(str1.compare(str2), 0);
}
TEST_F(BasicStringStrCompareStrTest, CompareCaseSensitive) {
    easystl::string str1("hello, world!");
    EXPECT_NE(str.compare(str1), 0);
}

// 2. int compare(size_type pos, size_type n, basic_string &str) const
class BasicStringSubStrCompareStrTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World!"; }
};
TEST_F(BasicStringSubStrCompareStrTest, CompareEqual) {
    easystl::string str1("Hello");
    EXPECT_EQ(str.compare(0, 5, str1), 0);
}
TEST_F(BasicStringSubStrCompareStrTest, CompareLessThan) {
    // not equal length
    easystl::string str1("Hello, World! Extra");
    EXPECT_LT(str.compare(0, 7, str1), 0);

    easystl::string str2("Hello, Worle! Extra");
    EXPECT_LT(str.compare(0, 13, str2), 0);

    // equal length
    easystl::string str3("Hello, Worlg!");
    EXPECT_LT(str.compare(0, str.size(), str3), 0);
}
TEST_F(BasicStringSubStrCompareStrTest, CompareGreaterThan) {
    // not equal length
    easystl::string str1("Hello, Worl");
    EXPECT_GT(str.compare(0, str.size(), str1), 0);

    easystl::string str2("Helln, World! Extra");
    EXPECT_GT(str.compare(0, 10, str2), 0);

    // equal length
    easystl::string str3("Hello, Worlc!");
    EXPECT_GT(str.compare(0, str.size(), str3), 0);
}
TEST_F(BasicStringSubStrCompareStrTest, CompareEmptyString) {
    easystl::string empty_str;
    EXPECT_GT(str.compare(0, 3, empty_str), 0);
    EXPECT_LT(empty_str.compare(0, 0, str), 0);
}
TEST_F(BasicStringSubStrCompareStrTest, CompareSameObject) {
    EXPECT_EQ(str.compare(0, str.size(), str), 0);
}
TEST_F(BasicStringSubStrCompareStrTest, CompareWithNullCharacter) {
    easystl::string str1("Hello\0World", 11);
    easystl::string str2("Hello\0Venus", 11);
    EXPECT_GT(str1.compare(0, str.size(), str2), 0);
}
TEST_F(BasicStringSubStrCompareStrTest, CompareCaseSensitive) {
    easystl::string str1("hello");
    EXPECT_NE(str.compare(0, 5, str1), 0);
}

// 3. int compare(size_type pos1, size_type n1, basic_string &str, size_type
// pos2, size_type n2 = npos)
class BasicStringSubStrCompareSubStrTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World!"; }
};
TEST_F(BasicStringSubStrCompareSubStrTest, CompareEqual) {
    easystl::string str1("Hello, World!");
    EXPECT_EQ(str.compare(8, 5, str1, 8, 5), 0);
}
TEST_F(BasicStringSubStrCompareSubStrTest, CompareLessThan) {
    // not equal length
    easystl::string str1("Hello, World! Extra");
    EXPECT_LT(str.compare(0, 7, str1, 0, 10), 0);

    easystl::string str2("Hello, Worle! Extra");
    EXPECT_LT(str.compare(0, 13, str2, 0), 0);

    // equal length
    easystl::string str3("Hello, Worlg!");
    EXPECT_LT(str.compare(0, str.size(), str3, 0), 0);
}
TEST_F(BasicStringSubStrCompareSubStrTest, CompareGreaterThan) {
    // not equal length
    easystl::string str1("Hello, Worl");
    EXPECT_GT(str.compare(0, str.size(), str1, 0, 5), 0);

    easystl::string str2("Helln, World! Extra");
    EXPECT_GT(str.compare(2, 8, str2, 2), 0);

    // equal length
    easystl::string str3("Hello, Worlc!");
    EXPECT_GT(str.compare(0, str.size(), str3, 0), 0);
}
TEST_F(BasicStringSubStrCompareSubStrTest, CompareEmptyString) {
    easystl::string empty_str;
    EXPECT_GT(str.compare(0, 3, empty_str, 0, 0), 0);
    EXPECT_LT(empty_str.compare(0, 0, str, 3, 2), 0);
}
TEST_F(BasicStringSubStrCompareSubStrTest, CompareSameObject) {
    EXPECT_EQ(str.compare(0, str.size(), str, 0, str.size()), 0);
}
TEST_F(BasicStringSubStrCompareSubStrTest, CompareWithNullCharacter) {
    easystl::string str1("Hello\0World", 11);
    easystl::string str2("Hello\0Venus", 11);
    EXPECT_GT(str1.compare(0, 8, str2, 0, 8), 0);
}
TEST_F(BasicStringSubStrCompareSubStrTest, CompareCaseSensitive) {
    easystl::string str1("hello");
    EXPECT_NE(str.compare(0, 5, str1, 0, 5), 0);
}

// 4. int compare(const CharType *s) const noexcept
class BasicStringStrCompareCStrTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World!"; }
};
TEST_F(BasicStringStrCompareCStrTest, CompareEqual) {
    EXPECT_EQ(str.compare("Hello, World!"), 0);
}
TEST_F(BasicStringStrCompareCStrTest, CompareLessThan) {
    // not equal length
    EXPECT_LT(str.compare("Hello, World! Extra"), 0);

    EXPECT_LT(str.compare("Hello, Worle! Extra"), 0);

    // equal length
    EXPECT_LT(str.compare("Hello, Worlg!"), 0);
}
TEST_F(BasicStringStrCompareCStrTest, CompareGreaterThan) {
    // not equal length
    EXPECT_GT(str.compare("Hello, Worl"), 0);

    EXPECT_GT(str.compare("Helln, World! Extra"), 0);

    // equal length
    easystl::string str3("Hello, Worlc!");
    EXPECT_GT(str.compare("Hello, Worlc!"), 0);
}
TEST_F(BasicStringStrCompareCStrTest, CompareEmptyString) {
    EXPECT_GT(str.compare(""), 0);
}
TEST_F(BasicStringStrCompareCStrTest, CompareSameObject) {
    EXPECT_EQ(str.compare(str.data()), 0);
}
TEST_F(BasicStringStrCompareCStrTest, CompareWithNullCharacter) {
    easystl::string str1("Hello\0World", 11);
    EXPECT_GT(str1.compare("Hello\0Venus"), 0);
}
TEST_F(BasicStringStrCompareCStrTest, CompareCaseSensitive) {
    EXPECT_NE(str.compare("hello, world!"), 0);
}

// 5. int compare(size_type pos, size_type n, const CharType *s)
class BasicStringSubStrCompareCStrTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World!"; }
};
TEST_F(BasicStringSubStrCompareCStrTest, CompareEqual) {
    EXPECT_EQ(str.compare(0, 5, "Hello"), 0);
}
TEST_F(BasicStringSubStrCompareCStrTest, CompareLessThan) {
    // not equal length
    EXPECT_LT(str.compare(0, 7, "Hello, World! Extra"), 0);

    EXPECT_LT(str.compare(0, 13, "Hello, Worle! Extra"), 0);

    // equal length
    EXPECT_LT(str.compare(0, str.size(), "Hello, Worlg!"), 0);
}
TEST_F(BasicStringSubStrCompareCStrTest, CompareGreaterThan) {
    // not equal length
    EXPECT_GT(str.compare(0, str.size(), "Hello, Worl"), 0);

    EXPECT_GT(str.compare(0, 10, "Helln, World! Extra"), 0);

    // equal length
    EXPECT_GT(str.compare(0, str.size(), "Hello, Worlc!"), 0);
}
TEST_F(BasicStringSubStrCompareCStrTest, CompareEmptyString) {
    EXPECT_GT(str.compare(0, 3, ""), 0);
}
TEST_F(BasicStringSubStrCompareCStrTest, CompareSameObject) {
    EXPECT_EQ(str.compare(0, str.size(), str.data()), 0);
}
TEST_F(BasicStringSubStrCompareCStrTest, CompareWithNullCharacter) {
    easystl::string str1("Hello\0World", 11);
    EXPECT_GT(str1.compare(0, str.size(), "Hello\0Venus"), 0);
}
TEST_F(BasicStringSubStrCompareCStrTest, CompareCaseSensitive) {
    EXPECT_NE(str.compare(0, 5, "hello"), 0);
}

// 6. int compare(size_type pos, size_type n1, const CharType *s, size_type n2)
class BasicStringSubStrCompareCSubStrTest : public ::testing::Test {
  protected:
    easystl::string str;

    void SetUp() override { str = "Hello, World!"; }
};
TEST_F(BasicStringSubStrCompareCSubStrTest, CompareEqual) {
    easystl::string str1("Hello, World!");
    EXPECT_EQ(str.compare(8, 5, "orld!", 5), 0);
}
TEST_F(BasicStringSubStrCompareCSubStrTest, CompareLessThan) {
    // not equal length
    easystl::string str1("Hello, World! Extra");
    EXPECT_LT(str.compare(0, 7, "Hello, World! Extra", 10), 0);

    easystl::string str2("Hello, Worle! Extra");
    EXPECT_LT(str.compare(0, 13, "Hello, Worle! Extra", 19), 0);

    // equal length
    easystl::string str3("Hello, Worlg!");
    EXPECT_LT(str.compare(0, str.size(), "Hello, Worlg!", 13), 0);
}
TEST_F(BasicStringSubStrCompareCSubStrTest, CompareGreaterThan) {
    // not equal length
    easystl::string str1("Hello, Worl");
    EXPECT_GT(str.compare(0, str.size(), "Hello, Worl", 5), 0);

    easystl::string str2("Helln, World! Extra");
    EXPECT_GT(str.compare(2, 8, "lln, World! Extra", 19), 0);

    // equal length
    easystl::string str3("Hello, Worlc!");
    EXPECT_GT(str.compare(0, str.size(), "Hello, Worlc!", 13), 0);
}
TEST_F(BasicStringSubStrCompareCSubStrTest, CompareEmptyString) {
    EXPECT_GT(str.compare(0, 3, "", 0), 0);
}
TEST_F(BasicStringSubStrCompareCSubStrTest, CompareSameObject) {
    EXPECT_EQ(str.compare(0, str.size(), str.data(), str.size()), 0);
}
TEST_F(BasicStringSubStrCompareCSubStrTest, CompareWithNullCharacter) {
    easystl::string str1("Hello\0World", 11);
    easystl::string str2("Hello\0Venus", 11);
    EXPECT_GT(str1.compare(0, 8, "Hello\0Venus", 8), 0);
}
TEST_F(BasicStringSubStrCompareCSubStrTest, CompareCaseSensitive) {
    easystl::string str1("hello");
    EXPECT_NE(str.compare(0, 5, "hello", 5), 0);
}
} // namespace compare_test

namespace operator_plus_test {
TEST(BasicStringOperatorPlusTest, StrConcatStr) {
    easystl::string str1("Hello ");
    easystl::string str2("World");
    easystl::string str3("");
    easystl::string str4("\0");
    EXPECT_EQ(str1 + str2, "Hello World");
    EXPECT_EQ(str1 + str3, "Hello ");
    EXPECT_EQ(str1 + str4, "Hello \0");
}
TEST(BasicStringOperatorPlusTest, CStrAndStr) {
    const char *str1 = "Hello ";
    easystl::string str2("World");
    easystl::string str3("");
    easystl::string str4("\0");
    EXPECT_EQ(str1 + str2, "Hello World");
    EXPECT_EQ(str1 + str3, "Hello ");

    easystl::string tmp1 = str1 + str4;
    EXPECT_EQ(tmp1.size(), 6);

    EXPECT_EQ(str2 + str1, "WorldHello ");
    EXPECT_EQ(str3 + str1, "Hello ");

    easystl::string tmp2 = str4 + str1;
    EXPECT_EQ(tmp2.size(), 6);
}
TEST(BasicStringOperatorPlusTest, CharAndStr) {
    const char ch = 'H';
    easystl::string str2("World");
    easystl::string str3("");
    easystl::string str4("\0");
    EXPECT_EQ(ch + str2, "HWorld");
    EXPECT_EQ(ch + str3, "H");

    easystl::string tmp1 = ch + str4;
    EXPECT_EQ(tmp1.size(), 1);

    EXPECT_EQ(str2 + ch, "WorldH");
    EXPECT_EQ(str3 + ch, "H");

    easystl::string tmp2 = str4 + ch;
    EXPECT_EQ(tmp2.size(), 1);
}
TEST(BasicStringOperatorPlusTest, MoveStrConcatStr) {
    easystl::string str1("Hello");
    easystl::string str2("World");
    EXPECT_EQ(easystl::move(str1) + str2, "HelloWorld");
    EXPECT_EQ(str1.size(), 0);
    EXPECT_NE(str2.size(), 0);
}
TEST(BasicStringOperatorPlusTest, MoveStrConcatCStr) {
    easystl::string str1("Hello");
    const char *str2 = "World";
    EXPECT_EQ(easystl::move(str1) + str2, "HelloWorld");
    EXPECT_EQ(str1.size(), 0);
}
TEST(BasicStringOperatorPlusTest, MoveStrConcatChar) {
    easystl::string str1("Hello");
    const char ch = '!';
    EXPECT_EQ(easystl::move(str1) + ch, "Hello!");
    EXPECT_EQ(str1.size(), 0);
}
TEST(BasicStringOperatorPlusTest, MoveStrConcatMoveStr) {
    easystl::string str1("Hello");
    easystl::string str2("World");
    EXPECT_EQ(easystl::move(str1) + easystl::move(str2), "HelloWorld");
    EXPECT_EQ(str1.size(), 0);
    EXPECT_NE(str2.size(), 0);
}
TEST(BasicStringOperatorPlusTest, CStrConcatMoveStr) {
    const char *str1 = "Hello";
    easystl::string str2("World");
    EXPECT_EQ(str1 + easystl::move(str2), "HelloWorld");
    EXPECT_EQ(str2.size(), 0);
}
TEST(BasicStringOperatorPlusTest, CharConcatMoveStr) {
    const char ch = 'H';
    easystl::string str2("World");
    EXPECT_EQ(ch + easystl::move(str2), "HWorld");
    EXPECT_EQ(str2.size(), 0);
}
TEST(BasicStringOperatorPlusTest, StrConcatMoveStr) {
    easystl::string str1("Hello");
    easystl::string str2("World");
    EXPECT_EQ(str1 + easystl::move(str2), "HelloWorld");
    EXPECT_NE(str1.size(), 0);
    EXPECT_EQ(str2.size(), 0);
}

} // namespace operator_plus_test
