#include <iostream>
#include "MemoryPool.hpp"

class T {
public:
    T() {
        std::cout << "构造函数" << std::endl;
    }
    ~T() {
        std::cout << "析构函数" << std::endl;
    }
};


int main() {

    auto p = new T[3];

    delete[] p;

    return 0;
}
