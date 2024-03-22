/*
*    Time: 05/09/22 11:29:07
*    Create By yongheng
*/

module;
#include <cstdio>
#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#elif defined(__linux__) || defined(__unix__)
#include <unistd.h>
#endif
export module ProgressBar;


export enum COLORS {
    BLACK,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LAKE_BLUE,
    REDDISH,
    PURPLE,
    YELLOW,
    WHITE,
    GRAY,
    BLUE,
    GREEN,
    SHALLOW_LIGHT_GREEN,
    RED,
    LAVENDER,
    SHALLOW_YELLOW,
    LIGHT_WHITE,
    NONE = LIGHT_WHITE
};

export void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    return;
}


//enum COLORS {
//    BLACK = 30,
//    RED,
//    GREEN,
//    YELLOW,
//    BLUE,
//    PURPLE,
//    WHITE = 37,
//    NONE = 0
//};
//
//void SetColor(int color) {
//    printf("\033[%dm", color);
//}
//
//void HideCursor() {
//    printf("\033[?25l");
//}
//void ShowCursor() {
//    printf("\033[?25h");
//}



export class ProgressBar {
    int _total;
    int _now;
    int _wide;
    int _percentage = 0;

public:
    ProgressBar(int now, int total) {
        _total = total - 1 < 0 ? 0 : total - 1;
        _now = now;
        _wide = 100;
    }
    void SetWide(int wide) {
        _wide = wide;
    }
    void SetTotal(int total) {
        _total = total;
    }
    void SetNow(int now) {
        _now = now;
    }
    int GetPercentage() const {
        return _now * 100 / _total;
    }
    std::string GetProgressBar() const {
        int percentage = GetPercentage();
        std::string progressbar(_wide > 2 ? _wide : 2, '-');

        for (int i = 1; i < _wide * percentage / 100; ++i) {
            progressbar[i] = '#';
        }
        progressbar[0] = '[';
        progressbar[progressbar.size() - 1] = ']';
        return progressbar;
    }
    ProgressBar& operator ++() {
        this->_now++;
        if (_now > _total)
            _now = _total;
        return *this;
    }
    ProgressBar operator ++(int) {
        auto cp = *this;
        _now++;
        if (_now > _total)
            _now = _total;
        return cp;
    }
};

export std::ostream& operator << (std::ostream& out, const ProgressBar &pb) {
    out << pb.GetProgressBar() << " " << pb.GetPercentage() << "%" << "\r";
    return out;
}



