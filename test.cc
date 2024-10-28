#include <iostream>
#include <iomanip>

typedef union {
    long int i;
    double d;
} DL;

int main() {
    DL dl;
    dl.d = -3.123283386230469e-05;
    std::cout << dl.d << " -> ";  // 0.01
    std::cout << std::hex << dl.i << std::endl;  // 3f847ae147ae147b

    dl.i = dl.i & 0xFFFFFC0000000000; // HSED中去掉正数的一部分尾数的方法

    std::cout << std::hex << dl.i << " -> ";  // 3f84780000000000
    std::cout << dl.d << std::endl;  // 0.00999451

    // Next 1
    dl.i += 0x40000000000;
    std::cout << "Next 1: " << dl.d << std::endl;

    return 0;
}
