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


export
NAMESPACE_BEGIN(nl)
enum class CountType {
    nanoseconds,
    microseconds,
    milliseconds,
    seconds,
    minutes,
    hours,
    days,
    weeks
};

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
    long count(CountType type = CountType::nanoseconds) {
        using namespace std::chrono;
        switch (type) {
        case CountType::nanoseconds:
            return duration_cast<nanoseconds>(time_point.time_since_epoch()).count();
        case CountType::microseconds:
            return duration_cast<microseconds>(time_point.time_since_epoch()).count();
        case CountType::milliseconds:
            return duration_cast<milliseconds> (time_point.time_since_epoch()).count();
        case CountType::seconds:
            return duration_cast<seconds> (time_point.time_since_epoch()).count();
        case CountType::minutes:
            return duration_cast<minutes> (time_point.time_since_epoch()).count();
        case CountType::hours:
            return duration_cast<hours> (time_point.time_since_epoch()).count();
        case CountType::days:
            return duration_cast<days> (time_point.time_since_epoch()).count();
        case CountType::weeks:
            return duration_cast<weeks> (time_point.time_since_epoch()).count();
        default:
            return -1;
        }
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


/*
 * 该Timer为在时间到达之后调用callback_func
 * @TODO 定时调用 callback_func
 */
template <bool en_thread = false>
class Timer {

    class TimerNode {
        using CallBack = std::function<void()>;
        CallBack callback_func_;
        time_t end_time_;

        template <bool>
        friend class Timer;

    public:
        TimerNode(CallBack func, size_t end_time) : callback_func_(func), end_time_(end_time) {  }

        bool operator < (const TimerNode& r) const {
            if (end_time_ < r.end_time_)
                return true;
            return false;
        }
    };

    std::multiset<TimerNode> tasks_;
    std::jthread thread_;

public:
    Timer() {
        if (en_thread == true) {
            thread_ = std::jthread(&Timer::start, this);
        }

    }
    void start() {
        while (!is_end()) {
            check_timer();
        }
    }
    void end() {
        thread_.request_stop();
    }
    bool is_end() {
        if (en_thread)
			return thread_.get_stop_token().stop_requested();
        return tasks_.empty();
    }
    TimerNode add_task(std::function<void()> func, time_t time) {
        TimerNode ret(func, GetNowTime() + time);
        tasks_.insert(ret);
        return ret;
    }
    TimerNode add_task(time_t time, auto&& func, auto &&...args) {
        auto fun = std::bind(std::forward<decltype(func)>(func), std::forward<decltype(args)>(args)...);
        return AddTask(fun, time);
    }
    void del_task(const TimerNode& del_timer_node) {
        auto it = tasks_.find(del_timer_node);
        if (it == tasks_.end())
            return;
        tasks_.erase(it);
    }

    bool check_timer() {
        if (tasks_.empty()) {
            end();
			return false;
        }
        if (auto it = tasks_.begin(); it->end_time_ < GetNowTime()) {
            it->callback_func_();
            tasks_.erase(it);
            return true;
        }

    }



};


NAMESPACE_END(nl)

