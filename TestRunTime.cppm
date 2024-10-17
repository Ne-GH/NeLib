/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/10/17 11:02
*******************************************************************************/

module;
#include <chrono>
#include <iostream>
#include <functional>

#include "tools.h"

export module TestRunTime;

import ProgressBar;

export
NAMESPACE_BEGIN(nl)

class TestRunTime {
    std::function<void()> test_func_;
    std::chrono::milliseconds run_time_{};
    size_t count_ = 0;
public:
    explicit TestRunTime(std::function<void()> func) : test_func_(std::move(func)) {  }
    void set_test_count(const size_t count) {
        count_ = count;
    }

    void start_test() {
        auto begin_time = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < count_; ++i) {
            test_func_();
        }
        auto end_time = std::chrono::high_resolution_clock::now();

        run_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
    }

    void start_test_and_show_progress() {
        nl::ProgressBar pb(count_);
        const auto begin_time = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < count_; ++i) {
            test_func_();
            ++pb;
            std::cout << pb;
        }
        const auto end_time = std::chrono::high_resolution_clock::now();

        run_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
    }
    size_t count() const {
        return run_time_.count();
    }



};


NAMESPACE_END()

