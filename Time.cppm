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


NAMESPACE_BEGIN(Date)
export class Date;

export class Year {
    friend Date;
    std::chrono::year year{};
public:
    explicit Year(int arg) {
        year = std::chrono::year(arg);
    }
    explicit Year(const char *arg) : Year(std::stoi(arg)) {  }

};
export class Month {
    friend Date;
    std::chrono::month month{};
public:
    explicit Month(int arg) {
        month = std::chrono::month(arg);
    }
    explicit Month(const char *arg) : Month(std::stoi(arg)) {  }
};
export class Day {
    friend Date;
    std::chrono::day day{};
public:
    explicit Day(int arg) {
        day = std::chrono::day(arg);
    }
    explicit Day(const char *arg) : Day(std::stoi(arg)) {  }


};
export class Date {
    std::chrono::year_month_day ymd{};
public:
    Date() {
        using namespace std::chrono;
        ymd = year_month_day(floor<days>(system_clock::now()));
    }
    explicit Date(const Year&& year,const Month&& month,const Day&& day) {
        ymd = std::chrono::year_month_day(year.year,month.month,day.day);
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
    friend std::ostream& operator << (std::ostream &out, Date &date);


};

std::ostream &operator<<(std::ostream &out, Date &date) {
    out << date.ymd;
    return out;
}

export NAMESPACE_BEGIN(DateLiterals)
Year operator "" _Y(const char *year) {
    return Year(year);
}

Month operator "" _M(const char *month) {
    return Month(month);
}

Day operator "" _D (const char *day) {
    return Day(day);
}
NAMESPACE_END(DateLiterals)
NAMESPACE_END(Date)

NAMESPACE_BEGIN(Timer)
export enum class CountType {
    nanoseconds,
    microseconds,
    milliseconds ,
    seconds ,
    minutes ,
    hours ,
    days,
    weeks
};

export class Timer {
    std::chrono::year_month_day ymd{};
    std::chrono::time_point<std::chrono::system_clock> time_point {};
    std::vector<decltype(time_point)> time_arr;
public:
    Timer() {
        time_point = std::chrono::system_clock::now();
    };
    // 暂时删除从日期构建Timer
/*
    explicit Timer(const Year&& year,const Month&& month,const Day&& day) {
        ymd = std::chrono::year_month_day(year.year,month.month,day.day);
        auto tmp = std::chrono::system_clock::now();
        std::cout << tmp << std::endl;
        time_point = std::chrono::time_point<std::chrono::system_clock>(std::chrono::sys_days(ymd));
        std::cout << time_point << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::hours>(tmp - time_point) << std::endl;
    }
    explicit Timer(std::chrono::year_month_day arg) : ymd(arg) {  }
    long CountTime(TimeType type = TimeType::DAY) {
        return std::chrono::sys_days(ymd).time_since_epoch().count();
    }
*/
    explicit Timer(std::chrono::time_point<std::chrono::system_clock> arg) : time_point(arg) {  }

    Timer operator - (Timer& t) {
        return Timer(std::chrono::time_point<std::chrono::system_clock>(time_point - t.time_point));
    }
    void FlagTimePoint() {
        time_arr.emplace_back(std::chrono::system_clock::now());
    }
    // 计时只考虑Start和End，Start前调用End无作用，Start后调用Start无作用
    // 重新计时需调用ReCount
    void Start() {
        if (time_arr.empty()) {
            FlagTimePoint();
        }
    }
    void End() {
        if (time_arr.size() == 1)
            FlagTimePoint();
    }
    void ReCount() {
        time_arr.clear();
    }
    long Count(CountType type = CountType::milliseconds) {
        if (time_arr.size() > 2)
            return -1;
        if (time_arr.empty()) {
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
        else
        switch (type) {
            case CountType::nanoseconds:
                return std::chrono::duration_cast<std::chrono::nanoseconds>(time_arr[1]-time_arr[0]).count();
            case CountType::microseconds:
                return std::chrono::duration_cast<std::chrono::microseconds>(time_arr[1]-time_arr[0]).count();
            case CountType::milliseconds:
                return std::chrono::duration_cast<std::chrono::milliseconds> (time_arr[1]-time_arr[0]).count();
            case CountType::seconds:
                return std::chrono::duration_cast<std::chrono::seconds> (time_arr[1]-time_arr[0]).count();
            case CountType::minutes:
                return std::chrono::duration_cast<std::chrono::minutes> (time_arr[1]-time_arr[0]).count();
            case CountType::hours:
                return std::chrono::duration_cast<std::chrono::hours> (time_arr[1]-time_arr[0]).count();
            case CountType::days:
                return std::chrono::duration_cast<std::chrono::days> (time_arr[1]-time_arr[0]).count();
            case CountType::weeks:
                return std::chrono::duration_cast<std::chrono::weeks> (time_arr[1]-time_arr[0]).count();
            default:
                return -1;
        }

    }

    void Now() {
        time_point = std::chrono::system_clock::now();
    }

    friend std::ostream & operator << (std::ostream &out,const Timer& timer);

};

std::ostream &operator << (std::ostream &out,const Timer& timer) {
    out << timer.time_point;
    return out;
}


NAMESPACE_END(Timer)


