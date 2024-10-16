/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/10/16 22:09
*******************************************************************************/

module;
#include "tools.h"
#include <opencv2/opencv.hpp>
#include <filesystem>
export module Image;

export NAMESPACE_BEGIN(nl)

class Image {
    cv::Mat image_;

public:
    Image() = default;
    explicit Image(const std::filesystem::path &&path);
    ~Image() = default;

    // @TODO 暂不支持拷贝操作
    Image(const Image &) = delete;
    Image &operator=(const Image &) = delete;

    [[nodiscard]] explicit operator bool() const;

    void open(const std::filesystem::path &&path) { image_ = cv::imread(path); }
    void save(const std::filesystem::path &&path) const { cv::imwrite(path, image_); }

    Image &zoom(double multiple);
    Image &set_image_width(int width);
    Image &set_image_height(int height);
    Image &resize(int width, int height);
    Image &rotation(int angle);
    Image &rotation(int x, int y, int angle);
    Image &reverse_horizontally();
    Image &reverse_vertically();
    Image &to_grayscale();
    Image &to_binary(int);
    Image &to_pseudo_color();
    std::vector<std::array<size_t, 256>> get_histogram_data();

    void show_test() const {
        cv::imshow("show_test", image_);
        cv::waitKey();
    }
};


NAMESPACE_END()




nl::Image::Image(const std::filesystem::path &&path) {
    image_ = cv::imread(path);
}

nl::Image::operator bool() const{
    return !image_.empty();
}

nl::Image& nl::Image::zoom(double multiple) {

    return *this;
}

nl::Image& nl::Image::set_image_width(int width) {

    return *this;
}

nl::Image& nl::Image::set_image_height(int height) {

    return *this;
}

nl::Image& nl::Image::resize(int width, int height) {

    return *this;
}

nl::Image& nl::Image::rotation(int angle) {

    return *this;
}

nl::Image& nl::Image::rotation(int x, int y, int angle) {

    return *this;
}

nl::Image& nl::Image::reverse_horizontally() {

    return *this;
}

nl::Image& nl::Image::reverse_vertically() {

    return *this;
}

nl::Image& nl::Image::to_grayscale() {

    return *this;
}

nl::Image& nl::Image::to_binary(int) {

    return *this;
}

nl::Image& nl::Image::to_pseudo_color() {

    return *this;
}

std::vector<std::array<size_t, 256>> nl::Image::get_histogram_data() {

    return {};
}
