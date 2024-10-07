/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/12/03
*******************************************************************************/
module;
#include "tools.h"
#include <chrono>
#include <set>
#include <thread>
#include <functional>
#include <algorithm>
#include <ranges>
#include <mutex>
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
    }

    explicit Time(const std::chrono::time_point<std::chrono::system_clock> arg) : time_point(arg) {  }

    Time operator - (const Time& t) const {
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
        time_t interval_duration;
    
        bool operator < (const TimerTask& right) const {
            if (end_time < right.end_time)
                return true;
            return false;
		}
	};

    void run_timer_task(std::set<TimerTask>::const_iterator &it) {
        if (it->is_repeat_task == true) {
            auto task = *it;
            it = tasks_.erase(it);
            task.callback_func();
            task.end_time += task.interval_duration;
            tasks_.insert(task);
        }
        else {
            it->callback_func();
            it = tasks_.erase(it);
        }

    }

    void run() {
        while (!is_finish()) {
            const auto cur_time = GetNowTime();
            for (auto it = tasks_.begin(); it != tasks_.end(); ) {
                if (it->end_time < cur_time) {
                    std::lock_guard lock(tasks_mutex_);
                    run_timer_task(it);
                }
                else
                    ++ it;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
        }

    }
    
    std::jthread thread_;
    std::mutex tasks_mutex_;
	std::multiset<TimerTask> tasks_;


public:
    Timer() {
        thread_ = std::jthread(&Timer::run, this);
    }

    [[nodiscard]]
    bool is_finish() const {
        return thread_.get_stop_token().stop_requested();
    }

    void add_task(CallbackFunc func, const time_t time, const bool is_repeat_task = false) {
		std::lock_guard lock(tasks_mutex_);
        tasks_.insert(
            { std::move(func) , GetNowTime() + time, is_repeat_task, time}
        );
    }
    void add_repeat_task(CallbackFunc func, const time_t time) {
        add_task(std::move(func), std::move(time), true);
    }
    void add_task(time_t time, auto&& func, auto &&...args) {
        auto fun = std::bind(std::forward<decltype(func)>(func), std::forward<decltype(args)>(args)...);
        return AddTask(fun, time);
    }

    size_t task_count() {
		std::lock_guard lock(tasks_mutex_);
        return tasks_.size();
    }
    size_t repeat_task_count() {
		std::lock_guard lock(tasks_mutex_);
        return std::ranges::count_if(tasks_, [](const TimerTask& task) {
            return !task.is_repeat_task;
            });
    }

};


NAMESPACE_END(nl)

