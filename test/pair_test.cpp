#include "./help_struct.h"
#include "utility.h"
#include "gtest/gtest.h"

TEST(PairTest, Constuctor) {
    easystl::pair<int, float> p1(42, 3.14);
    easystl::pair<int, float> p2(42, 3.14);
    // p1 = p2;
}
