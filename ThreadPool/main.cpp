#include <iostream>
#include <functional>
#include <any>
#include <functional>

void testFunc(const std::function<void()> call_func,int &arg) {
    std::cout << "void(int)" << std::endl;
}
void testFunc(const std::function<void(std::any &)> call_func,std::any &arg) {
    std::cout << "void(std::any)" << std::endl;
    call_func(arg);
}


int main(int argc, char** argv) {
    int num = 100;
    std::any a = num;
    auto lam = [](std::any& a) {
        std::cout << "lambda : " << a.type().name() << std::endl;
    };
//    std::function<void(int&)> call_int = std::bind(lam,std::placeholders::_1);
    std::function<void(std::any &)> call_any = std::bind(lam,std::placeholders::_1);

//    testFunc(call_int,10);
    testFunc(call_any,a);

}


