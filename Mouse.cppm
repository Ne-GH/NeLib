/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/09/22 12:49
*******************************************************************************/

module;
#include "tools.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <fstream>
#endif

#include <functional>
#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>
export module Mouse;


#ifdef _WIN32
constexpr int LeftButton = VK_LBUTTON;
constexpr int RightButton = VK_RBUTTON;

NAMESPACE_BEGIN(mouse_module_static)

bool GetKeyState(int key) {
    return GetKeyState(key) & 0x8000;
}
void SetCursorPos(int x,int y) {
    SetCursorPos(x , y);
}

void LeftClick() {
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void RightClick() {
    mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
}
NAMESPACE_END(mouse_module_static)

std::tuple<int,int> GetCursorPos() {
    POINT p;
    GetCursorPos(&p);
    return {p.x,p.y};
}


#else
constexpr int LeftButton = 1;
constexpr int RightButton = 2;
bool GetKeyState(int key) override {
    // @TODO
    return false;
}

void SetCursorPos(int x,int y) override {
    // @TODO
}

void LiftClick() {
    // @TODO
}

void RightClick() {
    // @TODO
}

std::tuple<int,int> GetCursorPos() override {
    std::ifstream in("/dev/input/mice", std::ifstream::in);
    unsigned char data[3];
    in.read(reinterpret_cast<char *>(data),3);
    return {data[1],data[2]};
}

#endif





class ClickEvent {
    bool state = false;
public:
    bool operator ()(const bool clicked) {
        const bool ignore = clicked == state;
        state = clicked;
        if (clicked && !ignore)
            return true;
        return false;
    }
};

export class Mouse {

public:
    struct CursorPos {
        int x = 0;
        int y = 0;
        bool operator==(const CursorPos& right) const {
            return x == right.x && y == right.y;
        }

    };

private:

    CursorPos old_cursor_pos_;
    std::thread listen_click_thread_;
    bool run_flag_ = true;
    std::mutex thread_mutex_;
    std::function<void()> left_click_callback_;
    std::function<void()> right_click_callback_;
    std::function<void()> move_callback_;

    static CursorPos get_current_cursor_pos() {
        const auto pos = GetCursorPos();
        return { std::get<0>(pos),std::get<1>(pos) };
    }

    [[noreturn]]
    void listen_event() {
        ClickEvent right_click_event;
        ClickEvent left_click_event;

        while (true) {
            {
                std::lock_guard lock(thread_mutex_);
                if (!run_flag_) {
                    return;
                }

            }
            if (left_click_event(GetKeyState(LeftButton))) {
                if (left_click_callback_)
                    left_click_callback_();
            }
            if (right_click_event(GetKeyState(RightButton))) {
                if (right_click_callback_)
                    right_click_callback_();
            }
            if (const auto cur_cursor_pos = get_current_cursor_pos(); cur_cursor_pos != old_cursor_pos_) {
                old_cursor_pos_ = cursor_pos = cur_cursor_pos;

                if (move_callback_)
                    move_callback_();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        }
    }
public:
    CursorPos cursor_pos;
    Mouse() : Mouse(get_current_cursor_pos()){  }
    explicit Mouse(const CursorPos cursor_pos) : cursor_pos(cursor_pos) {
        old_cursor_pos_ = cursor_pos;
        listen_click_thread_ = std::thread(&Mouse::listen_event, this);
    }
    ~Mouse() {
        {
            std::lock_guard<std::mutex> lock(thread_mutex_);
            run_flag_ = false;

        }
        listen_click_thread_.detach();

    }
    void move_to(const CursorPos pos) {
        cursor_pos = pos;
        SetCursorPos(pos.x , pos.y);//更改鼠标坐标
    }
    void lift_click(const CursorPos pos) {
        cursor_pos = pos;
        mouse_module_static::LeftClick();
    }
    void lift_click(int count = 1) {
        while (count --)
            lift_click(cursor_pos);
    }
    void right_click(const CursorPos pos) {
        cursor_pos = pos;
        mouse_module_static::RightClick();
    }
    void right_click(int count = 1) {
        while (count --)
            right_click(cursor_pos);
    }

    void set_left_click_callback(std::function<void()> callback) {
        left_click_callback_ = std::move(callback);
    }
    void set_right_click_callback(std::function<void()> callback) {
        right_click_callback_ = std::move(callback);
    }
    void set_move_callback(std::function<void()> callback) {
        move_callback_ = std::move(callback);
    }


};

export
std::ostream& operator << (std::ostream& out, const Mouse::CursorPos& pos) {
    out << pos.x << ' ' << pos.y;
    return out;
}
