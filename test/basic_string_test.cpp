#include "astring.h"
#include "gtest/gtest.h"
#include <climits>
#include <cstring>

TEST(StringTest, ConstructorTest) {
    // no parameter constructor
    easystl::string s1;
    EXPECT_EQ(s1.capacity(), 32);
    EXPECT_EQ(s1.size(), 0);
    EXPECT_NE(s1.begin(), nullptr);

    // construct string with size and character
    easystl::string s2(10, 'c');
    EXPECT_EQ(s2.capacity(), 32);
    EXPECT_EQ(s2.size(), 10);
    EXPECT_EQ(s2[4], 'c');

    // construct string with other c-style string
    easystl::string s3("hello");
    EXPECT_EQ(s3.capacity(), 32);
    EXPECT_EQ(s3.size(), 5);
    EXPECT_EQ(s3[4], 'o');

    easystl::string s4("hello", 3);
    EXPECT_EQ(s4.capacity(), 32);
    EXPECT_EQ(s4.size(), 3);
    EXPECT_EQ(s4[2], 'l');

    // construct string with other string
    easystl::string s5(s3, 2);
    EXPECT_EQ(s5.capacity(), 32);
    EXPECT_EQ(s5.size(), 3);
    EXPECT_EQ(s5[2], 'o');

    easystl::string s6(s3, 2, 2);
    EXPECT_EQ(s6.capacity(), 32);
    EXPECT_EQ(s6.size(), 2);
    EXPECT_EQ(s6[0], s5[1]);

    // construct string from a range
    easystl::string s7(s3.begin(), s3.end());
    EXPECT_EQ(s7.capacity(), 32);
    EXPECT_EQ(s7.size(), s3.size());
    EXPECT_EQ(s7[4], s3[4]);

    // copy constructor
    easystl::string s8(s7);
    EXPECT_EQ(s8.capacity(), 32);
    EXPECT_EQ(s8.size(), s3.size());
    EXPECT_EQ(s8[4], s3[4]);

    // copy constructor
    easystl::string s9(easystl::move(s7));
    EXPECT_EQ(s9.capacity(), 32);
    EXPECT_EQ(s9.size(), s3.size());
    EXPECT_EQ(s9[4], s3[4]);
    EXPECT_EQ(s7.begin(), nullptr);
}

TEST(StringTest, AssignTest) {
    easystl::string s1("hello");

    // copy assignment
    easystl::string s2;
    s2 = s1;
    EXPECT_EQ(s2[3], s1[3]);

    // move assignment
    easystl::string s3;
    s3 = easystl::move(s2);
    EXPECT_EQ(s3[3], s1[3]);
    EXPECT_EQ(s2.begin(), nullptr);

    // assign with c-style string
    easystl::string s4;
    s4 = "hello";
    EXPECT_EQ(s4[3], s1[3]);

    // assign with character
    easystl::string s5;
    s5 = 'i';
    EXPECT_EQ(s5[0], 'i');
}

TEST(StringTest, IteratorTest) {
    const easystl::string s1("hello");
    EXPECT_EQ(*s1.begin(), 'h');
    EXPECT_EQ(*s1.end(), '\0');

    // reverse iterator
}

TEST(StringTest, CapacityTest) {
    const easystl::string s1("hello");
    EXPECT_EQ(s1.empty(), false);
    EXPECT_EQ(s1.capacity(), 32);
    EXPECT_EQ(s1.size(), 5);
    EXPECT_EQ(s1.length(), 5);

    easystl::string s2("hello, world");
    EXPECT_EQ(s2.size(), 12);
    s2.shrink_to_fit();
    EXPECT_EQ(s2.capacity(), s2.size());
    s2.reserve(16);
    EXPECT_EQ(s2.capacity(), 16);
}

TEST(StringTest, AccessTest) {
    const easystl::string s1("hello");
    EXPECT_EQ(s1[3], 'l');
    bool res = 0;
    try {
        s1.at(10);
        res = 1;
    } catch (...) {
    }
    EXPECT_EQ(res, false);

    EXPECT_EQ(s1.front(), 'h');
    EXPECT_EQ(s1.back(), 'o');

    EXPECT_EQ(s1.data(), s1.begin());
    EXPECT_EQ(s1.data(), s1.c_str());
}

TEST(StringTest, InsertTest) {
    easystl::string s1("hello");

    s1.insert(s1.begin() + 0, 'a');
    EXPECT_EQ(s1, easystl::string("ahello"));
    s1.insert(s1.begin() + s1.length(), 'a');
    EXPECT_EQ(s1, easystl::string("ahelloa"));
    s1.insert(s1.begin() + 3, 'a');
    EXPECT_EQ(s1, easystl::string("ahealloa"));

    easystl::string s2("hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh");
    s2.insert(s2.begin(), 'h');
    s2.insert(s2.begin(), 'a');
    EXPECT_EQ(s2, easystl::string("ahhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"));

    s1 = "hello";
    s1.insert(s1.begin(), 0, 'a');
    EXPECT_EQ(s1, easystl::string("hello"));
    s1.insert(s1.end() + 2, 2, 'a');
    // EXPECT_EQ(s1, easystl::string("helloaa"));

    // EXPECT_EQ(*(s1.insert(s1.begin(), s1.begin(), s1.end()) + 13), 'h');
    // EXPECT_EQ(s1.length(), 16);
    // EXPECT_EQ(s1, easystl::string("helllllohelllllo"));
    //
}

TEST(StringTest, PushAndPopTest) {
    easystl::string s1("hello");
    s1.push_back(',');
    EXPECT_EQ(s1.back(), ',');
    s1.pop_back();
    EXPECT_NE(s1.back(), ',');
}

TEST(StringTest, AppendTest) {
    easystl::string s1("hello");
    s1.append(2, 'o');
    EXPECT_EQ(s1, easystl::string("hellooo"));

    easystl::string s2("hello");
    s2.append(s1, 2, 2);
    EXPECT_EQ(s2, easystl::string("helloll"));

    const char *cstr = "world";
    easystl::string s3 = "hello";
    s3.append(cstr, strlen(cstr));

    easystl::string s4 = "hello";
    s4.append(s4);
    EXPECT_EQ(s4, easystl::string("hellohello"));

    easystl::string s5 = "hello";
    s5.append(s4, 5);
    EXPECT_EQ(s5, easystl::string("hellohello"));

    easystl::string s6 = "hello";
    s6.append(cstr);
    EXPECT_EQ(s6, easystl::string("helloworld"));

    easystl::string s7 = "hello";
    s7.append(s7.begin(), s7.end());
    EXPECT_EQ(s7, easystl::string("hellohello"));
}

TEST(StringTest, ClearTest) {
    easystl::string s1("hello");
    s1.erase(s1.begin() + 1);
    EXPECT_EQ(s1, easystl::string("hllo"));

    s1.erase(s1.begin(), s1.end() - 2);
    EXPECT_EQ(s1, easystl::string("lo"));

    s1.clear();
    EXPECT_EQ(s1, easystl::string());
}

TEST(StringTest, ResizeTest) {
    easystl::string s1("hello");
    s1.resize(10, 'o');
    EXPECT_EQ(s1.size(), 10);

    s1.resize(12);
    EXPECT_EQ(s1.size(), 12);
    EXPECT_EQ(s1.at(11), '\0');
}

TEST(StringTest, SubstrTest) {
    easystl::string s1("hello");
    easystl::string s2 = s1.substr(1, 3);
    EXPECT_EQ(s2, easystl::string("ell"));

    easystl::string s3 = s1.substr(3);
    EXPECT_EQ(s3, easystl::string("lo"));
}

TEST(StringTest, ReplaceTest) {
    easystl::string s1("hello, world");
    s1.replace(7, 5, easystl::string("C++"));
    EXPECT_EQ(s1, easystl::string("hello, C++"));

    s1.replace(s1.begin() + 7, s1.end(), easystl::string("world"));
    EXPECT_EQ(s1, easystl::string("hello, world"));

    s1.replace(7, 5, "C++");
    EXPECT_EQ(s1, easystl::string("hello, C++"));

    s1.replace(s1.begin() + 7, s1.end(), "world");
    EXPECT_EQ(s1, easystl::string("hello, world"));

    s1.replace(7, 5, "C++Rust", 3);
    EXPECT_EQ(s1, easystl::string("hello, C++"));

    s1.replace(s1.begin() + 7, s1.end(), "worldworld", 5);
    EXPECT_EQ(s1, easystl::string("hello, world"));

    s1.replace(7, 5, 3, 'C');
    EXPECT_EQ(s1, easystl::string("hello, CCC"));

    s1.replace(s1.begin() + 7, s1.end(), 3, 'R');
    EXPECT_EQ(s1, easystl::string("hello, RRR"));

    s1.replace(7, 3, easystl::string("worldworld"), 5);
    EXPECT_EQ(s1, easystl::string("hello, world"));

    s1.replace(s1.begin() + 7, s1.end(), s1.begin(), s1.begin() + 5);
    EXPECT_EQ(s1, easystl::string("hello, hello"));
}

TEST(StringTest, ReverseTest) {
    easystl::string s1("ABCDEF");
    s1.reverse();
    EXPECT_EQ(s1, easystl::string("FEDCBA"));
}

TEST(StringTest, SwapTest) {
    easystl::string s1("abcdef");
    easystl::string s2("ABCDEF");
    s1.swap(s2);

    EXPECT_EQ(s1, easystl::string("ABCDEF"));
    EXPECT_EQ(s2, easystl::string("abcdef"));
}

TEST(StringTest, FindTest) {
    easystl::string s1("abcdef");
    EXPECT_EQ(s1.find('c'), 2);
    EXPECT_EQ(s1.find('g'), easystl::string::npos);

    EXPECT_EQ(s1.find("cde"), 2);
    EXPECT_EQ(s1.find("cde", 4), easystl::string::npos);
    EXPECT_EQ(s1.find("efg"), easystl::string::npos);

    EXPECT_EQ(s1.find("cde", 2, 2), 2);
    EXPECT_EQ(s1.find("cde", 4, 1), easystl::string::npos);

    easystl::string s2("bcd");
    EXPECT_EQ(s1.find(s2), 1);
    EXPECT_EQ(s1.find(s2, 2), easystl::string::npos);
}

TEST(StringTest, RfindTest) {
    easystl::string s1("This is an apple which is a delicious fruit");
    EXPECT_EQ(s1.rfind('i'), 41);
    EXPECT_EQ(s1.rfind('i', 10), 5);
    EXPECT_EQ(s1.rfind('g'), easystl::string::npos);

    EXPECT_EQ(s1.rfind("is"), 23);
    EXPECT_EQ(s1.rfind("is", 10), 5);

    EXPECT_EQ(s1.rfind("is an", 30, 4), 23);
    EXPECT_EQ(s1.rfind("is an", 30, 5), 5);

    easystl::string s2("is");
    EXPECT_EQ(s1.rfind(s2), 23);
    EXPECT_EQ(s1.rfind(s2, 10), 5);
}

TEST(StringTest, FindFirstOfTest) {
    EXPECT_EQ(easystl::string("co_await").find_first_of('a'), 3);

    EXPECT_EQ(easystl::string("consteval").find_first_of("xyzabc", 0, 3),
              easystl::string::npos);
    EXPECT_EQ(easystl::string("consteval").find_first_of("xyzabc", 0, 6), 0);

    EXPECT_EQ(easystl::string("alignas").find_first_of("klmn"), 1);
    EXPECT_EQ(easystl::string("alignas").find_first_of("wxyz"),
              easystl::string::npos);

    EXPECT_EQ(easystl::string("consteval").find_first_of("xyzabc"), 0);
}

TEST(StringTest, FindFirstNotOfTest) {
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
    EXPECT_EQ(data, easystl::string("1) %DONE%, 2) %DONE%, 3) %DONE%"));
}

TEST(StringTest, FindLastOfTest) {
    EXPECT_EQ(easystl::string("co_await").find_last_of('c'), 0);
    EXPECT_EQ(easystl::string("co_await").find_last_of('x'),
              easystl::string::npos);

    EXPECT_EQ(easystl::string("consteval").find_last_of("xyzabc", 0, 3),
              easystl::string::npos);
    EXPECT_EQ(easystl::string("consteval").find_last_of("xyzabc", 0, 6), 7);

    EXPECT_EQ(easystl::string("alignas").find_last_of("klmn"), 4);
    EXPECT_EQ(easystl::string("alignas").find_last_of("wxyz"),
              easystl::string::npos);

    EXPECT_EQ(
        easystl::string("consteval").find_last_of(easystl::string("xyzabc")),
        7);
    EXPECT_EQ(
        easystl::string("consteval").find_last_of(easystl::string("xyz"), 1),
        easystl::string::npos);
}

TEST(StringTest, FindLastNotOfTest) {
    EXPECT_EQ(easystl::string("co_await").find_last_not_of('t'), 6);
    EXPECT_EQ(easystl::string("aaa").find_last_not_of('a'),
              easystl::string::npos);

    EXPECT_EQ(easystl::string("abcdefg").find_last_not_of("efg", 3, 2),
              6); // 'g'

    EXPECT_EQ(easystl::string("abcdefg").find_last_not_of("efg", 3),
              3); // 'd'
    EXPECT_EQ(easystl::string("abcdefg").find_last_not_of("efg"),
              3); // 'd'

    EXPECT_EQ(
        easystl::string("abcdefg").find_last_not_of(easystl::string("efg"), 3),
        3); // 'd'
    EXPECT_EQ(
        easystl::string("abcdefg").find_last_not_of(easystl::string("efg")),
        3); // 'd'
}

TEST(StringTest, CountTest) {
    easystl::string s1("This is an apple which is a delicious fruit");
    EXPECT_EQ(s1.count('i'), 7);
    EXPECT_EQ(s1.count('x'), 0);
}

TEST(StringTest, OperatorPlusTest) {
    easystl::string s1("hello ");
    easystl::string s2("world");

    EXPECT_EQ(s1 + s2, easystl::string("hello world"));
    EXPECT_EQ("hello " + s2, easystl::string("hello world"));
    EXPECT_EQ(' ' + s2, easystl::string(" world"));
    EXPECT_EQ(s1 + "world", easystl::string("hello world"));
    EXPECT_EQ(s1 + 'w', easystl::string("hello w"));
    EXPECT_EQ(easystl::move(s1) + s2, easystl::string("hello world"));
    s1 = "hello ";
    EXPECT_EQ(s1 + easystl::move(s2), easystl::string("hello world"));
    s2 = "world";
    EXPECT_EQ(easystl::move(s1) + easystl::move(s2),
              easystl::string("hello world"));

    // s2 = "world";
    // EXPECT_EQ(s2, easystl::string("world"));
    // EXPECT_EQ("hello " + easystl::move(s2), easystl::string("hello world"));
    // s2 = "world";
    // EXPECT_EQ(' ' + easystl::move(s2), easystl::string(" world"));
    //
    // s1 = "hello ";
    // EXPECT_EQ(easystl::move(s1) + "world", easystl::string("hello world"));
    // s1 = "hello ";
    // EXPECT_EQ(easystl::move(s1) + 'w', easystl::string("hello w"));
}

TEST(StringTest, OperatorPlusEqualTest) {
    easystl::string s1("hello");
    s1 += 'o';
    s1 += "world";
    s1 += s1;
    EXPECT_EQ(s1, easystl::string("hellooworldhellooworld"));
}

TEST(StringTest, OperatorOutputTest) {
    easystl::string s1("hello");
    std::cout << s1 << std::endl;
}
