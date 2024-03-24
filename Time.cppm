/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/12/03
*******************************************************************************/

module;
#include <iostream>
#include <chrono>
#include <set>
#include <thread>
#include <functional>
export module Time;

#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END(name) }

NAMESPACE_BEGIN(Time)
export class Date;

export class Year {
    friend Date;
    std::chrono::year year{};
public:
    explicit Year(int arg) {
        year = std::chrono::year(arg);
    }
    explicit Year(const char* arg) : Year(std::stoi(arg)) {  }

};
export class Month {
    friend Date;
    std::chrono::month month{};
public:
    explicit Month(int arg) {
        month = std::chrono::month(arg);
    }
    explicit Month(const char* arg) : Month(std::stoi(arg)) {  }
};
export class Day {
    friend Date;
    std::chrono::day day{};
public:
    explicit Day(int arg) {
        day = std::chrono::day(arg);
    }
    explicit Day(const char* arg) : Day(std::stoi(arg)) {  }


};
export class Time;
export class Date {
    friend class Time;
    std::chrono::year_month_day ymd{};
public:
    Date() {
        using namespace std::chrono;
        ymd = year_month_day(floor<days>(system_clock::now()));
    }
    explicit Date(const Year&& year, const Month&& month, const Day&& day) {
        ymd = std::chrono::year_month_day(year.year, month.month, day.day);
        auto tmp = std::chrono::system_clock::now();
        std::cout << tmp << std::endl;
    }
    long Day() {
        return ymd.day().operator unsigned int();
    }
    long Month() {
        return ymd.month().operator unsigned int();
    }
    long Year() {
        return ymd.year().operator int();
    }
    bool IsLeap() {
        return ymd.year().is_leap();
    }


    explicit Date(std::chrono::year_month_day arg) : ymd(arg) {  }
    friend std::ostream& operator << (std::ostream& out, Date& date);


};

std::ostream& operator<<(std::ostream& out, Date& date) {
    out << date.ymd;
    return out;
}

export NAMESPACE_BEGIN(DateLiterals)
Year operator "" _Y(const char* year) {
    return Year(year);
}

Month operator "" _M(const char* month) {
    return Month(month);
}

Day operator "" _D(const char* day) {
    return Day(day);
}
NAMESPACE_END(DateLiterals)

export enum class CountType {
    nanoseconds,
    microseconds,
    milliseconds,
    seconds,
    minutes,
    hours,
    days,
    weeks
};

export class Time {
    std::chrono::time_point<std::chrono::system_clock> time_point{};
public:
    Time() {
        time_point = std::chrono::system_clock::now();
    };
    explicit Time(Date arg) {
        time_point = std::chrono::time_point<std::chrono::system_clock>(std::chrono::sys_days(arg.ymd));
    }

    explicit Time(std::chrono::time_point<std::chrono::system_clock> arg) : time_point(arg) {  }

    Time operator - (Time& t) {
        return Time(std::chrono::time_point<std::chrono::system_clock>(time_point - t.time_point));
    }
    long Count(CountType type = CountType::nanoseconds) {
        switch (type) {
        case CountType::nanoseconds:
            return std::chrono::duration_cast<std::chrono::nanoseconds>(time_point.time_since_epoch()).count();
        case CountType::microseconds:
            return std::chrono::duration_cast<std::chrono::microseconds>(time_point.time_since_epoch()).count();
        case CountType::milliseconds:
            return std::chrono::duration_cast<std::chrono::milliseconds> (time_point.time_since_epoch()).count();
        case CountType::seconds:
            return std::chrono::duration_cast<std::chrono::seconds> (time_point.time_since_epoch()).count();
        case CountType::minutes:
            return std::chrono::duration_cast<std::chrono::minutes> (time_point.time_since_epoch()).count();
        case CountType::hours:
            return std::chrono::duration_cast<std::chrono::hours> (time_point.time_since_epoch()).count();
        case CountType::days:
            return std::chrono::duration_cast<std::chrono::days> (time_point.time_since_epoch()).count();
        case CountType::weeks:
            return std::chrono::duration_cast<std::chrono::weeks> (time_point.time_since_epoch()).count();
        default:
            return -1;
        }
    }

    void Now() {
        time_point = std::chrono::system_clock::now();
    }

    Date ToDate() {
        using namespace std::chrono;
        return Date(std::chrono::year_month_day{ floor<days>(system_clock::now()) });
    }
    friend std::ostream& operator << (std::ostream& out, Time Time) {
        out << Time.Count();
        return out;
    }

};

export class TimerNode {
    using CallBack = std::function<void()>;
    CallBack _func;
    time_t _end_time;
    template <bool >
    friend class Timer;

public:
    TimerNode(CallBack func, size_t end_time) : _func(func), _end_time(end_time) {  }

    bool operator < (const TimerNode& r) const {
        if (_end_time < r._end_time)
            return true;
        return false;
    }
};


time_t GetNowTime() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

export
template <bool en_thread = false>
class Timer {
    std::multiset<TimerNode> _tasks;
    std::jthread _thread;
public:
    Timer() {
        if (en_thread == true) {
            _thread = std::jthread(&Timer::Start, this);
        }

    }
    void Start() {
        while (!IsEnd()) {
            CheckTimer();
        }
    }
    void End() {
        _thread.request_stop();
    }
    bool IsEnd() {
        return _thread.get_stop_token().stop_requested();
    }
    TimerNode AddTask(std::function<void()> func, time_t time) {
        TimerNode ret(func, GetNowTime() + time);
        _tasks.insert(ret);
        return ret;
    }
    TimerNode AddTask(time_t time, auto&& func, auto &&...args) {
        auto fun = std::bind(std::forward<decltype(func)>(func), std::forward<decltype(args)>(args)...);
        return AddTask(fun, time);
    }
    void DelTask(const TimerNode& del_timer_node) {
        auto it = _tasks.find(del_timer_node);
        if (it == _tasks.end())
            return;
        _tasks.erase(it);
    }

    bool CheckTimer() {
        auto it = _tasks.begin();
        if (it == _tasks.end())
            return false;
        if (it->_end_time < GetNowTime()) {
            it->_func();
            _tasks.erase(it);
            return true;
        }
    }



};



NAMESPACE_END(Time)
