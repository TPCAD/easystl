#include "utility.h"
#include "vector.h"
#include "gtest/gtest.h"
#include <climits>

TEST(VectorTest, Constructor) {
    // non-arguments Constructor
    easystl::vector<int> vec1;
    EXPECT_EQ(vec1.capacity(), 16);

    // Constructor with size
    easystl::vector<int> vec2(10);
    EXPECT_GE(vec2.capacity(), 16);
    EXPECT_EQ(vec2.size(), 10);
    EXPECT_EQ(vec2[5], 0);

    // Constructor with size and init value
    easystl::vector<int> vec3(10, 20);
    EXPECT_GE(vec3.capacity(), 16);
    EXPECT_EQ(vec3.size(), 10);
    EXPECT_EQ(vec3[5], 20);

    // Constructor with range of other vector's
    easystl::vector<int> src{1, 2, 3, 4, 5, 6, 7, 9, 10};
    easystl::vector<int> vec4(src.begin(), src.begin());
    EXPECT_EQ(vec4.capacity(), 16);
    EXPECT_EQ(vec4.size(), 0);

    easystl::vector<int> vec5(src.begin(), src.begin() + 5);
    EXPECT_EQ(vec5.capacity(), 16);
    EXPECT_EQ(vec5.size(), 5);
    EXPECT_EQ(vec5[0], 1);

    // Copy constructor
    easystl::vector<int> vec6(vec5);
    EXPECT_EQ(vec6, vec5);

    // Move constructor
    easystl::vector<int> vec7(easystl::move(vec5));
    EXPECT_EQ(vec7, vec6);
    EXPECT_EQ(vec5.begin(), nullptr);

    // Copy assignment
    easystl::vector<int> vec8 = vec6;
    EXPECT_EQ(vec8, vec6);

    // Move constructor
    easystl::vector<int> vec9 = easystl::move(vec6);
    EXPECT_EQ(vec9, vec8);
    EXPECT_EQ(vec6.begin(), nullptr);
}

TEST(VectorTest, IteratorTest) {
    easystl::vector<int> vec1{1, 2, 3, 4, 5, 6, 7, 9, 10};
    EXPECT_EQ(vec1.begin(), vec1.data());
    EXPECT_EQ(*vec1.begin(), 1);
    EXPECT_EQ(*(vec1.begin() + 4), 5);
    EXPECT_EQ(vec1.end(), vec1.data() + 9);
    EXPECT_EQ(*(vec1.end() - 4), 6);

    const easystl::vector<int> vec2{1, 2, 3, 4, 5, 6, 7, 9, 10};
    EXPECT_EQ(*vec2.begin(), 1);
    EXPECT_EQ(*(vec2.end() - 1), 10);

    // reverse iterator
}

TEST(VectorTest, CapacityTest) {
    // empty()
    easystl::vector<int> empty_vec;
    easystl::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 9, 10};
    EXPECT_EQ(empty_vec.empty(), true);

    // size()
    EXPECT_EQ(empty_vec.size(), 0);
    EXPECT_EQ(vec.size(), 9);

    // max_size()

    // capacity()
    easystl::vector<int> large_vec(20, 4);
    EXPECT_EQ(empty_vec.capacity(), 16);
    EXPECT_EQ(large_vec.capacity(), 20);

    // reserve()
    empty_vec.reserve(4);
    large_vec.reserve(40);
    EXPECT_EQ(empty_vec.capacity(), 16);
    EXPECT_EQ(large_vec.capacity(), 40);

    // shrink_to_fit
    large_vec.shrink_to_fit();
    EXPECT_EQ(large_vec.capacity(), 20);
}

TEST(VectorTest, AccessTest) {
    easystl::vector<int> vec1{1, 23, 3, 4, 5, 5, 6, 6};
    EXPECT_EQ(vec1[2], 3);
    // EXPECT_EQ(vec1[9], 3); // error
    EXPECT_EQ(vec1.at(2), 3);
    // EXPECT_EQ(vec1.at(9), 3); // error, out of range
    EXPECT_EQ(vec1.front(), 1);
    EXPECT_EQ(vec1.back(), 6);
}

TEST(VectorTest, DataTest) {
    easystl::vector<int> vec1{1, 23, 3, 4, 5, 5, 6, 6};
    EXPECT_EQ(vec1.data(), &vec1.front());
}

TEST(VectorTest, AssignTest) {
    // assign with size and value
    easystl::vector<int> vec1{1, 23, 3, 4, 5, 5, 6, 6};
    vec1.assign(10, 42);
    EXPECT_EQ(vec1.capacity(), 16);
    EXPECT_EQ(vec1.size(), 10);
    EXPECT_EQ(vec1.at(6), 42);

    // assign with start and end iterator
    easystl::vector<int> vec2{1, 23, 3, 4, 5, 5, 6, 6};
    vec2.assign(vec1.begin(), &vec1.back());
    EXPECT_EQ(vec2.capacity(), 16);
    EXPECT_EQ(vec2.size(), 9);
    EXPECT_EQ(vec2.at(6), 42);

    // assign with initializer list
    easystl::vector<int> vec3{1, 23, 3, 4, 5, 5, 6, 6};
    vec3.assign({1, 2, 3, 4, 5, 6, 7, 8, 9});
    EXPECT_EQ(vec3.capacity(), 16);
    EXPECT_EQ(vec3.size(), 9);
    EXPECT_EQ(vec3.at(6), 7);
}

TEST(VectorTest, EmplaceTest) {
    easystl::vector<int> vec1{1, 23, 3, 4, 5, 5, 6, 6};
    EXPECT_EQ(*vec1.emplace(vec1.begin() + 2, 42), 42);
    vec1.emplace_back(24);
    EXPECT_EQ(vec1.back(), 24);
}

TEST(VectorTest, PushAndPopTest) {
    // push
    easystl::vector<int> vec1{1, 23, 3, 4, 5, 5, 6, 6};
    int elem = 24;
    vec1.push_back(elem);
    EXPECT_EQ(vec1.back(), 24);
    vec1.push_back(42);
    EXPECT_EQ(vec1.back(), 42);

    // pop
    vec1.pop_back();
    EXPECT_NE(vec1.back(), 42);
}

TEST(VectorTest, InsertTest) {
    easystl::vector<int> vec1{1, 23, 3, 4, 5, 5, 6, 6};
    int elem = 24;
    vec1.insert(vec1.begin(), elem);
    EXPECT_EQ(vec1.front(), 24);
    vec1.insert(vec1.begin(), 42);
    EXPECT_EQ(vec1.front(), 42);

    // insert n times
    easystl::vector<int> vec2{1, 23, 3, 4, 5, 5, 6, 6};
    EXPECT_EQ(vec2.size(), 8);
    vec2.insert(vec2.begin(), 4, 42);
    EXPECT_EQ(vec2.size(), 12);

    // insert elements from vector
    easystl::vector<int> vec3{1, 23, 3, 4, 5, 5, 6, 6};
    EXPECT_EQ(vec3.size(), 8);
    vec3.insert(vec3.begin(), vec3.begin() + 1, vec3.begin() + 3);
    EXPECT_EQ(vec3.size(), 10);
}

TEST(VectorTest, EraseTest) {
    // erase one element
    easystl::vector<int> vec1{1, 23, 3, 4, 5, 5, 6, 6};
    EXPECT_EQ(vec1.at(1), 23);
    vec1.erase(vec1.begin() + 1);
    EXPECT_NE(vec1.at(1), 23);

    // erase elements of range
    EXPECT_EQ(vec1.size(), 7);
    vec1.erase(vec1.end() - 4, vec1.end());
    EXPECT_EQ(vec1.size(), 3);

    // erase all elements
    vec1.clear();
    EXPECT_EQ(vec1.size(), 0);
    EXPECT_EQ(vec1.capacity(), 16);
}

TEST(VectorTest, ResizeTest) {
    // narrow
    easystl::vector<int> vec1{1, 23, 3, 4, 5, 5, 6, 6};
    EXPECT_EQ(vec1.size(), 8);
    vec1.resize(4, 42);
    EXPECT_EQ(vec1.size(), 4);

    // expand
    easystl::vector<int> vec2{1, 23, 3, 4, 5, 5, 6, 6};
    EXPECT_EQ(vec2.size(), 8);
    vec2.resize(10, 42);
    EXPECT_EQ(vec2.size(), 10);
    EXPECT_EQ(vec2.at(9), 42);
}

TEST(VectorTest, ReverseTest) {
    easystl::vector<int> vec1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec1.reverse();
    for (auto i = 0UL; i < vec1.size(); ++i) {
        EXPECT_EQ(vec1[i], vec1.size() - i - 1);
    }
}

TEST(VectorTest, SwapTest) {
    easystl::vector<int> vec1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto vec2 = vec1;
    vec1.reverse();
    vec1.swap(vec2);
    for (auto i = 0UL; i < vec1.size(); ++i) {
        EXPECT_EQ(vec1[i], i);
    }
}
