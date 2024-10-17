/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/10/16 22:09
*******************************************************************************/

module;
#include "tools.h"
#include <opencv2/opencv.hpp>
#include <filesystem>
export module Image;
import MultArray;

export NAMESPACE_BEGIN(nl)

class Image {
    cv::Mat image_;

    template<typename T>
    constexpr size_t get_elem_size();

    struct BGRPixel {
        uchar B, G, R;
    };
    enum {
        B, G, R
    };


public:
    Image() = default;
    explicit Image(const std::filesystem::path &&path);
    explicit Image(const cv::Mat &image) : image_(image) {  };
    ~Image() = default;

    // @TODO 暂不支持拷贝操作
    Image(const Image &) = delete;
    Image &operator=(const Image &) = delete;

    [[nodiscard]]
    explicit operator bool() const {
        return !image_.empty();
    }

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
    cv::Mat get_histogram(int = 100,int = 100);

    void show_test() const {
        cv::imshow("show_test", image_);
        cv::waitKey();
    }
};


NAMESPACE_END()


template<typename T>
constexpr size_t nl::Image::get_elem_size() {
    if constexpr (std::is_same_v<T,BGRPixel>) 
        return 3;
    else if (std::is_same_v<T,uchar>)
        return 1;
}


template<typename T>
std::tuple<nl::MultArray<T>,int,int> GetImageData(const cv::Mat &image) {
    nl::MultArray<T> data(reinterpret_cast<T *>(image.data), { image.rows, image.cols });
    return { data, image.rows, image.cols };
}


nl::Image::Image(const std::filesystem::path &&path) {
    image_ = cv::imread(path);
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

    auto [data, row, col] = GetImageData<BGRPixel>(image_);

    auto image = cv::Mat(row, col, CV_8UC1);
    nl::MultArray<uchar> image_data(image.data, { row,col });

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            auto pixel = data({ i,j });
            int gray = pixel.R * 0.299 + pixel.G * 0.587 + pixel.B * 0.114;
            image_data({ i,j }) = gray;
        }
    }

    image_ = image;
    return *this;
}

nl::Image& nl::Image::to_binary(int threshold) {
    int elem_size = image_.elemSize();

    // @TODO 未处理RGB图像
    if (elem_size != 1)
        return *this;

    int row = image_.rows, col = image_.cols;
    nl::MultArray<uchar> data(image_.data, { row, col });


    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (data({ i, j }) > threshold)
                data({ i, j }) = 255;
            else
                data({ i, j }) = 0;
        }
    }
    return *this;
}

nl::Image& nl::Image::to_pseudo_color() {

    return *this;
}

std::vector<std::array<size_t, 256>> nl::Image::get_histogram_data() {
    size_t pixel_size = image_.elemSize();

    // 灰度图
    if (pixel_size == 1) {
        std::array<size_t, 256> count = { 0 };
        auto [data, row, col] = GetImageData<uchar>(image_);
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                count[data({ i, j })]++;
            }
        }
        return { count };
    }
    // 彩色图 imread 读取类型为 BGR
    else if (pixel_size == 3) {
        std::vector<std::array<size_t, 256>> count(3);
        auto [data, row, col] = GetImageData<BGRPixel>(image_);
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                (count[B])[data({ i,j }).B]++;
                (count[G])[data({ i,j }).G]++;
                (count[R])[data({ i,j }).R]++;
            }
        }
        return count;
    }
}

cv::Mat nl::Image::get_histogram(int width ,int height) {
    auto arrs = get_histogram_data();

    cv::Mat ret(height,width,CV_8UC3,cv::Scalar(255,255,255));
    const double unit_width = width * 1.0 / 256;

    if (arrs.size() == 1) {
        auto arr = arrs[0];
        int max = *std::max_element(arr.begin(),arr.end());

        for (int i = 0;i < 256; ++i) {
            int cur_height = static_cast<int>(arr[i] * 1.0 / max * height);

            cv::rectangle(
                ret,
                {static_cast<int>(i * unit_width), height - cur_height},
                {static_cast<int>((i + 1) * unit_width), height},
                cv::Scalar(0, 0, 0),
                unit_width
                );
        }
        return ret;
    }
    else if (arrs.size() == 3) {
        int max1 = *std::ranges::max_element(arrs[0]);
        int max2 = *std::ranges::max_element(arrs[1]);
        int max3 = *std::ranges::max_element(arrs[2]);

        int max = max1;
        if (max < max2)
            max = max2;
        if (max < max3)
            max = max3;

        auto func = [&](const int index,const cv::Scalar& color) {
            for (int i = 1;i < 256; ++i) {
                const int prev_height = static_cast<int>(arrs[index][i - 1] * 1.0 / max * height);
                const int cur_height = static_cast<int>(arrs[index][i] * 1.0 / max * height);
                cv::line(ret,
                    {static_cast<int>(i * unit_width),height - prev_height},
                    {static_cast<int>((i + 1) * unit_width),height - cur_height},
                    color,
                    1,
                    cv::LINE_AA
                    );
            }
        };
        func(0,cv::Scalar(255,0,0));
        func(1,cv::Scalar(0,255,0));
        func(2,cv::Scalar(0,0,255));

        return ret;
    }

    return ret;
}
