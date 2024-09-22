/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/09/22 12:49
*******************************************************************************/

module;
#include <functional>
#include <thread>
#include <utility>
export module Mouse;

export class Mouse {
public:
    struct CursorPos {
        int x = 0;
        int y = 0;
        bool operator==(const CursorPos &right) const {
            return x == right.x && y == right.y;
        }
    };
private:

    CursorPos old_cursor_pos_;
    std::thread listen_click_thread_;
    std::function<void()> left_click_callback_;
    std::function<void()> right_click_callback_;
    std::function<void()> move_callback_;
    static CursorPos get_current_cursor_pos() {
        // @TODO 获取当前鼠标坐标
        return {};
    }

    [[noreturn]]
    void listen_click() {
        while (true) {
            // @TODO
            // 监听按键状态
            // 如果按键被按下
            if (true) {
                if (left_click_callback_)
                    left_click_callback_();
                if (right_click_callback_)
                    right_click_callback_();
                if (move_callback_) {
                    move_callback_();
                }
            }
            if (const auto cur_cursor_pos = get_current_cursor_pos(); cur_cursor_pos != old_cursor_pos_) {
                old_cursor_pos_ = cur_cursor_pos;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        }
    }
public:
    CursorPos cursor_pos;
    Mouse() {
        // @TODO 获取当前鼠标坐标
        cursor_pos.x = 0;
        cursor_pos.y = 0;
        new (this) Mouse(cursor_pos);
    }
    explicit Mouse(const CursorPos cursor_pos) : cursor_pos(cursor_pos) {
        old_cursor_pos_ = cursor_pos;
        listen_click_thread_ = std::thread(&Mouse::listen_click, this);
    }

    void move_to(const CursorPos pos) {
        cursor_pos = pos;
        // @TODO
        // set cursor pos
    }
    void lift_click(const CursorPos pos) {
        cursor_pos = pos;
        // @TODO
        // click
    }
    void lift_click() {
        lift_click(cursor_pos);
    }
    void right_click(const CursorPos pos) {
        cursor_pos = pos;
        // @TODO
        // click

    }
    void right_click() {
        right_click(cursor_pos);
    }

    void set_lift_click_callback(std::function<void()> callback) {
        left_click_callback_ = std::move(callback);
    }
    void set_right_click_callback(std::function<void()> callback) {
        right_click_callback_ = std::move(callback);
    }
    void set_move_callback(std::function<void()> callback) {
        move_callback_ = std::move(callback);
    }


};
