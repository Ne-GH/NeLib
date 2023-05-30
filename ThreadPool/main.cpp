#include <iostream>
#include <vector>
#include <chrono>

#include "ThreadPool.hpp"

using std::cout,std::cin,std::endl;

template<typename First,typename ...Args>
void Print(First first, Args ...args) {
    cout << first;
    auto print = [](const auto &args) {
        cout << ' ' << args;
    };
    (..., print(args));
}

int calc(int x)
{
    if (x <= 2)
        return 1;
    return calc(x - 1) + calc(x - 2);
}


int main(int argc, char* argv[]) {
    ThreadPool pool(4);
    auto result = pool.Enqueue([](int answer) { return answer; }, 42);
    cout << pool.Enqueue(calc,4).get() << std::endl;

    std::cout << result.get() << std::endl;
    return 0;
}