/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/12/03
*******************************************************************************/

module;
#include <iostream>
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
    explicit Year(int arg) {
        year = std::chrono::year(arg);
    }
    explicit Year(const char *arg) : Year(std::stoi(arg)) {  }

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


export enum class TimeType {
    YEAR,MONTH,DAY
};

export class Timer {
    std::chrono::year_month_day ymd{};
    std::chrono::time_point<std::chrono::system_clock> time_point {};

public:
    explicit Timer(const Year&& year,const Month&& month,const Day&& day) {
        ymd = std::chrono::year_month_day(year.year,month.month,day.day);
        auto tmp = std::chrono::system_clock::now();
        std::cout << tmp << std::endl;
        time_point = std::chrono::time_point<std::chrono::system_clock>(std::chrono::sys_days(ymd));
        std::cout << time_point << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::hours>(tmp - time_point) << std::endl;
    }
    explicit Timer(std::chrono::year_month_day arg) : ymd(arg) {  }

    long Count(TimeType type = TimeType::DAY) {
        return std::chrono::sys_days(ymd).time_since_epoch().count();
    }
    long operator - (Timer& t) {
        return Count() - t.Count();
    }

    friend std::ostream & operator << (std::ostream &out,Timer timer);

};

export Timer Now() {
    auto p = std::chrono::system_clock::now();
    using namespace std::chrono;
    return Timer(std::chrono::year_month_day{floor<days>(system_clock::now())});
}
std::ostream &operator << (std::ostream &out,const Timer timer) {
    out << timer.ymd;
    return out;
}


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


