/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/10/20 15:58
*******************************************************************************/

module;
#include <opencv2/opencv.hpp>
#include "tools.h"

import Image;
import Camera;
export module Video;

export
NAMESPACE_BEGIN(nl)

class Video {
    cv::VideoWriter video_;
public:
    Video(const std::string &&video_name, const int width, const int height) {
        video_ = cv::VideoWriter(video_name, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 20, cv::Size(width, height));
    }
    explicit Video(const std::string &&video_name ,Camera camera) : Video("video.avi",camera.width(), camera.height()) {  }

    void push_back(Image image) {
        video_.write(image.get_mat());
    }
    void push_back(const cv::Mat &image) {
        video_.write(image);
    }
    void save() {
        video_.release();
    }
    ~Video() {
        video_.release();
    }

};


NAMESPACE_END(nl)
