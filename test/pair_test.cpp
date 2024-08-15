#include "utility.h"
int foo() {
    constexpr easystl::pair<int, double> p(42, 3.14);
    if (p.first == 42) {
        return 0;
    } else {
        return 1;
    }
}
int bar() {
    constexpr easystl::pair<int, double> p(42, 3.14);
    if (p.second == 1.14) {
        return 0;
    } else {
        return 1;
    }
}
int main() {
    foo();
    bar();
}
