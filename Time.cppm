/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/12/03
*******************************************************************************/
module;
#include "tools.h"
#include <iostream>
#include <chrono>
#include <set>
#include <thread>
#include <functional>
export module Time;

time_t GetNowTime() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}


template<class T>
concept CountType = std::is_same_v<T, std::chrono::nanoseconds>
|| std::is_same_v<T, std::chrono::microseconds>
|| std::is_same_v<T, std::chrono::milliseconds>
|| std::is_same_v<T, std::chrono::seconds>
|| std::is_same_v<T, std::chrono::minutes>
|| std::is_same_v<T, std::chrono::hours>
|| std::is_same_v<T, std::chrono::days>
|| std::is_same_v<T, std::chrono::weeks>;

export
NAMESPACE_BEGIN(nl)


class Time {
    std::chrono::time_point<std::chrono::system_clock> time_point{};
public:
    Time() {
        time_point = std::chrono::system_clock::now();
    };

    explicit Time(std::chrono::time_point<std::chrono::system_clock> arg) : time_point(arg) {  }

    Time operator - (Time& t) {
        return Time(std::chrono::time_point<std::chrono::system_clock>(time_point - t.time_point));
    }
    
    template<CountType T = std::chrono::milliseconds>
    size_t count() {
        return std::chrono::duration_cast<T>(time_point.time_since_epoch()).count();
    }

    Time& to_now() {
        time_point = std::chrono::system_clock::now();
        return *this;
    }

    friend std::ostream& operator << (std::ostream& out, Time time) {
        out << std::format("{:%T}", time.time_point);
        return out;
    }

};


class Timer {

    using CallbackFunc = std::function<void()>;
    
    struct TimerTask {
        CallbackFunc callback_func;
        time_t end_time;
        bool is_repeat_task = false;
    
    
        bool operator < (const TimerTask& right) const {
            if (end_time < right.end_time)
                return true;
            return false;
		}
	};

    void run() {
        while (!is_finish()) {
            
            auto cur_time = GetNowTime();
            for (auto it = tasks_.begin(); it != tasks_.end(); ) {
                if (it->end_time < cur_time) {
                    it->callback_func();
                    it = tasks_.erase(it);
                }
                else
                    ++ it;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
        }

    }
    
	std::multiset<TimerTask> tasks_;
    std::jthread thread_;


public:
    Timer() {
        thread_ = std::jthread(&Timer::run, this);
    }

    bool is_finish() {
        return thread_.get_stop_token().stop_requested();
    }

    void add_task(CallbackFunc func, time_t time, bool is_repeat_task = false) {
        tasks_.insert(
            { .callback_func = func, .end_time = GetNowTime() + time, .is_repeat_task = is_repeat_task}
        );
    }
    void add_task(time_t time, auto&& func, auto &&...args) {
        auto fun = std::bind(std::forward<decltype(func)>(func), std::forward<decltype(args)>(args)...);
        return AddTask(fun, time);
    }



};


NAMESPACE_END(nl)

