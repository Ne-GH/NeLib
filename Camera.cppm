/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/10/17 21:30
*******************************************************************************/

module;

#include <opencv2/opencv.hpp>

#include "tools.h"

export module Camera;

export
NAMESPACE_BEGIN(nl)

class Camera {
    cv::VideoCapture camera_;

    class iterator {
        cv::Mat image_;
        Camera *parent_;
    public:
        explicit iterator(Camera* parent = nullptr) : parent_(parent) {
            if (!parent_)
                return;

            do {
                parent_->camera_ >> image_;
            } while (image_.empty());
        }

        iterator& operator++() {
            do {
                parent_->camera_ >> image_;
            } while (image_.empty());

            return *this;
        }
        bool operator != (const iterator& other) const {
            return true;
        }
        cv::Mat& operator*() {
            return image_;
        }

    };
    class flip_iterator {
        iterator it_;
    public:
        explicit flip_iterator(Camera* parent = nullptr) : it_(parent) {  }
        flip_iterator& operator++() {
            ++ it_;
            return *this;
        }
        bool operator != (const flip_iterator& other) const {
            return it_ != other.it_;
        }
        cv::Mat& operator*() {
            cv::flip(*it_,*it_,1);
            return *it_;
        }


    };

public:
    Camera() {
#ifdef _WIN32
        camera_ = cv::VideoCapture(0);
#elif __linux__
        camera_ = cv::VideoCapture(0, cv::CAP_V4L2);
#endif

        if (!camera_.isOpened()) {
            throw std::runtime_error("can't open camera");
        }
    }

    iterator begin() {
        return iterator(this);
    }
    iterator end() {
        return iterator(nullptr);
    }
    flip_iterator flip_begin() {
        return flip_iterator(this);
    }
    flip_iterator flip_end() {
        return flip_iterator(nullptr);
    }
    size_t get_fps() const {
        auto cur_frame = camera_.get(cv::CAP_PROP_POS_FRAMES);
        auto cur_time = camera_.get(cv::CAP_PROP_POS_MSEC);
        return cur_frame / (cur_time / 1000);
    }


    static void for_each_wait() {
        // 120 fps : wait 8
        // 60  fps : wait 16
        // 30  fps : wait 33
        // 15  fps : wait 66

        cv::waitKey(8);
    }
    void show(const std::string &&window_name) {
        for (auto it = begin(); it != end(); ++it) {
            cv::imshow(window_name, *it);
            for_each_wait();
        }
    }
    void flip_show(const std::string &&window_name) {
        for (auto it = flip_begin(); it != flip_end(); ++it) {
            cv::imshow(window_name, *it);
            for_each_wait();
        }
    }
    cv::Mat get_frame() {
        return *iterator(this);
    }

};


NAMESPACE_END()
