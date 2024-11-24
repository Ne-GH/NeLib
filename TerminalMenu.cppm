/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/11/24 11:48
*******************************************************************************/

module;
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "tools.h"
export module TerminalMenu;


export
NAMESPACE_BEGIN(nl)

class TerminalMenu {
    struct Options {
        std::string title;
        std::function<void()> action;
    };
    std::vector<Options> menu_;
public:
    friend std::ostream& operator<<(std::ostream& os, const TerminalMenu& menu) {
        for (const auto& [title, action] : menu.menu_) {
            std::cout << title << std::endl;
        }
        return os;
    }

    TerminalMenu() { }
    void add_menu(std::string name, std::function<void()> func) {
        menu_.emplace_back(Options {name, func});
    }

    void start() {
        while (true) {
            // 清屏
            system("clear");
            std::cout << *this;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};


NAMESPACE_END(nl)



