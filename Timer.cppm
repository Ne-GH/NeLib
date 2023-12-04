/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/12/03
*******************************************************************************/

module;
#include <chrono>
export module Timer;

#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END(name) }


NAMESPACE_BEGIN(Timer)
export class Timer;

export class Year {
    friend Timer;
    std::chrono::year year{};
public:
    Year(int arg) {
        year = std::chrono::year(arg);
    }
    Year(const char *arg) : Year(std::stoi(arg)) {  }

};
export class Month {
    friend Timer;
    std::chrono::month month{};
public:
    explicit Month(int arg) {
        month = std::chrono::month(arg);
    }
    explicit Month(const char *arg) : Month(std::stoi(arg)) {  }
};
export class Day {
    friend Timer;
    std::chrono::day day{};
public:
    explicit Day(int arg) {
        day = std::chrono::day(arg);
    }
    explicit Day(const char *arg) : Day(std::stoi(arg)) {  }


};



export class Timer {
    std::chrono::year_month_day ymd{};

public:
    explicit Timer(const Year&& year,const Month&& month,const Day&& day) {
        ymd = std::chrono::year_month_day(year.year,month.month,day.day);
    }

    int Count() {
        return std::chrono::sys_days(ymd).time_since_epoch().count();
    }


};

export NAMESPACE_BEGIN(TimerLiterals)
Year operator "" _Y(const char *year) {
    return Year(year);
}

Month operator "" _M(const char *month) {
    return Month(month);
}

Day operator "" _D (const char *day) {
    return Day(day);
}
NAMESPACE_END(TimerLiterals)
NAMESPACE_END(Timer)


