/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/10/16 22:09
*******************************************************************************/

module;
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include "tools.h"
export module Image;
import MultArray;

#define USE_OPENCV_LIB true

export
NAMESPACE_BEGIN(nl)

class Image {
    cv::Mat image_;

    struct BGRPixel {
        uchar B, G, R;
    };

    void add(const cv::Scalar &);
    // Image &add(const Image&);
    void sub(const cv::Scalar &);
    // Image &sub(const Image&);
    void mul(const cv::Scalar &);
    // Image &mul(const Image&);
    void div(const cv::Scalar &);
    // Image &div(const Image&);

public:
    enum { B, G, R };

    int row{};
    int col{};
    Image() = default;
    explicit Image(const std::string &path) { open(path); }

    explicit Image(const cv::Mat &image) : image_(image), row(image_.rows), col(image_.cols){};

    Image(int width, int height, int type = CV_8UC3) { image_ = cv::Mat(height, width, type); }

    Image(const Image &image, int x, int y, int width, int height) {
        cv::Rect rect(x, y, width, height);
        image_ = cv::Mat(image.image_, rect);
        row = image_.rows;
        col = image_.cols;
    }

    Image &operator=(const cv::Mat &image) {
        image_ = image;
        return *this;
    }
    Image &operator=(const cv::Scalar &scalar) {
        image_ = scalar;
        return *this;
    }

    Image &clone_from(const Image &other) {
        image_ = other.image_.clone();
        other.image_.copyTo(image_);
        return *this;
    }

    [[nodiscard]] explicit operator bool() const { return !image_.empty(); }

    void open(const std::string &path) {
        image_ = cv::imread(path);
        if (image_.empty())
            throw std::runtime_error("can't open image");
        row = image_.rows;
        col = image_.cols;
    }
    void save(const std::string &path) const { cv::imwrite(path, image_); }

    cv::Mat get_mat() { return image_; }

    void show(const std::string &window_name) const { cv::imshow(window_name, image_); }

    void show_and_wait(const std::string &window_name) const {
        show(window_name);
        cv::waitKey();
    }

    static auto get_window(const std::string &window_name) { return cvGetWindowHandle(window_name.data()); }

    static void set_mouse_callback(const std::string &window_name, cv::MouseCallback callback) {
        cv::setMouseCallback(window_name, callback);
    }

    Image operator+(const cv::Scalar &scalar) const;
    Image operator-(const cv::Scalar &scalar) const;
    Image operator*(const cv::Scalar &scalar) const;
    Image operator/(const cv::Scalar &scalar) const;
    Image &operator+=(const cv::Scalar &scalar);
    Image &operator-=(const cv::Scalar &scalar);
    Image &operator*=(const cv::Scalar &scalar);
    Image &operator/=(const cv::Scalar &scalar);

    Image operator~() const;
    Image operator&(const Image &other) const;
    Image operator|(const Image &other) const;
    Image operator^(const Image &other) const;
    Image &operator&=(const Image &other);
    Image &operator|=(const Image &other);
    Image &operator^=(const Image &other);


    Image &zoom(double multiple);
    Image &set_image_width(int width);
    Image &set_image_height(int height);
    Image &resize(int width, int height,int inter_type = cv::INTER_LINEAR);
    Image &rotation(int angle, bool resize = false);
    Image &rotation(double x, double y, int angle, bool resize = false);
    Image &reverse_horizontally();
    Image &reverse_vertically();
    Image &to_grayscale(); // 转灰度
    Image &to_hsv(); // 转hsv
    Image &to_binary(int); // 转二值图
    Image &to_pseudo_color(); // 转伪彩色
    Image &to_blur(); // 转模糊
    Image &to_normalize(double, double, int); // 归一化
    Image &set_brightness(int); // 设置亮度
    Image &set_saturation(double); // 设置饱和度
    Image &set_contrast(double); // 设置对比度

    Image &draw_line(cv::Point, cv::Point, const cv::Scalar &, int);
    Image &draw_circle(cv::Point, int, cv::Scalar, int);
    Image &draw_ellipse(cv::RotatedRect, cv::Scalar, int);
    Image &draw_rect(cv::Rect, cv::Scalar, int);
    Image &draw_polyline(const std::vector<cv::Point> &, cv::Scalar, int, bool = true);
    Image &draw_contours(const std::vector<cv::Point> &, cv::Scalar, int);
    Image &draw_contours(const std::vector<std::vector<cv::Point>> &contours, cv::Scalar, int);


    std::vector<Image> split_channels() const;
    Image get_channel_image(int) const;
    Image get_inrange(cv::Scalar, cv::Scalar) const;

    std::vector<double> get_mean() const;
    std::vector<double> get_stddev() const;

    std::vector<std::array<size_t, 256>> get_histogram_data() const;
    Image get_histogram(int = 100, int = 100);
};


Image &Image::to_normalize(double alpha, double beat, int type){
    image_.convertTo(image_, CV_32F);
    cv::normalize(image_, image_, alpha, beat, type);
    return *this;
}
NAMESPACE_END(nl)


template <typename T>
std::tuple<nl::MultArray<T>,int,int> GetImageData(const cv::Mat &image) {
    nl::MultArray<T> data(reinterpret_cast<T *>(image.data), { image.rows, image.cols });
    return { data, image.rows, image.cols };
}

nl::Image& nl::Image::zoom(double multiple) {
#ifdef USE_OPENCV_LIB
    int new_width = image_.cols * multiple;
    int new_height = image_.rows * multiple;
    cv::resize(image_, image_, cv::Size(new_width, new_height));
#else
    if (image_.empty() || multiple == 1)
        return *this;

    auto func = [&]<typename T>() {
        const int new_width = static_cast<int>(image_.cols * multiple);
        const int new_height = static_cast<int>(image_.rows * multiple);
        auto new_image = cv::Mat(new_height, new_width, image_.type());
        auto [src_data, src_row, src_col] = GetImageData<T>(image_);
        auto [new_data, ignore1, ignore2] = GetImageData<T>(new_image);

        for (int i = 0; i < new_height; ++i)
            for (int j = 0; j < new_width; ++j)
                new_data(i,j) = src_data({ static_cast<int>(i / multiple),static_cast<int>(j / multiple) });

        image_ = new_image;
    };

    // 填充像素
    if (image_.elemSize() == 1)
        func.operator()<uchar>();
    else if (image_.elemSize() == 3)
        func.operator()<BGRPixel>();

#endif
    return *this;
}

nl::Image& nl::Image::set_image_width(int width) {
#ifdef USE_OPENCV_LIB
    int new_width = image_.cols * width;
    cv::resize(image_,image_,cv::Size(new_width,image_.rows));
#else
    if (image_.empty())
        return *this;
    double multiple = width * 1.0 / image_.cols;

    auto func = [&]<typename T>() {
        int row = image_.rows, col = image_.cols * multiple;
        auto new_image = cv::Mat(row, col, image_.type());
        auto [src_data, _1, _2] = GetImageData<T>(image_);
        auto [new_data, _3, _4] = GetImageData<T>(new_image);

        for (int i = 0; i < row; ++i)
            for (int j = 0; j < col; ++j)
                new_data(i,j) = src_data({ i,static_cast<int>(j / multiple) });

        image_ = new_image;
    };

    if (image_.elemSize() == 1)
        func.operator() < uchar > ();
    else if (image_.elemSize() == 3)
        func.operator() < BGRPixel > ();
#endif
    return *this;
}

nl::Image& nl::Image::set_image_height(int height) {
#ifdef USE_OPENCV_LIB
    int new_height = image_.rows * height;
    cv::resize(image_,image_,cv::Size(image_.cols,new_height));
#else
    if (image_.empty())
        return *this;
    double multiple = height * 1.0 / image_.rows;

    auto func = [&]<typename T>() {
        int row = image_.rows * multiple, col = image_.cols;
        auto new_image = cv::Mat(row, col, image_.type());
        auto [src_data, src_row, src_col] = GetImageData<T>(image_);
        auto [new_data, ignore1, ignore2] = GetImageData<T>(new_image);

        for (int i = 0; i < row; ++i)
            for (int j = 0; j < col; ++j)
                new_data(i,j) = src_data({ static_cast<int>(i / multiple), j });

        image_ = new_image;
    };
    if (image_.elemSize() == 1)
        func.operator() < uchar > ();
    else if (image_.elemSize() == 3)
        func.operator() < BGRPixel > ();
#endif
    return *this;
}

nl::Image& nl::Image::resize(int width, int height,int inter_type) {
#ifdef USE_OPENCV_LIB
    cv::resize(image_, image_, cv::Size(width, height),0,0, inter_type);
#else
    set_image_width(width);
    set_image_height(height);
#endif
    return *this;
}

nl::Image& nl::Image::rotation(int angle, bool resize) {
    rotation(image_.cols / 2, image_.rows / 2, angle, resize);
    return *this;
}

nl::Image& nl::Image::rotation(double x, double y, int angle, bool resize) {
#ifdef USE_OPENCV_LIB
    cv::Mat matrix = cv::getRotationMatrix2D(cv::Point2f(x,y), angle, 1.0);

    if (resize) {
        double angle_rad = angle * CV_PI / 180;
        double abs_cos = std::abs(std::cos(angle_rad));
        double abs_sin = std::abs(std::sin(angle_rad));
        int new_width = image_.cols * abs_cos + image_.rows * abs_sin;
        int new_height = image_.rows * abs_cos + image_.cols * abs_sin;
        matrix.at<double>(0,2) += (new_width - image_.cols) / 2;
        matrix.at<double>(1,2) += (new_height - image_.rows) / 2;

        cv::warpAffine(image_, image_, matrix, {new_width, new_height});
    }
    else {
        cv::warpAffine(image_, image_, matrix, image_.size());
    }

#else
    auto rotation = [&] <typename T>{
        auto [src_data, row, col] = GetImageData<T>(image_);
        double angle_rad = angle * CV_PI / 180;
        cv::Mat image(row, col, image_.type());
        auto [dest_data, _1, _2] = GetImageData<T>(image);
        /*
            dest.x = (src.x - x) * cos(angle) - (src.y - y) * sin(angle) + x
            dest.y = (src.x - x) * sin(angle) + (src.y - y) * cos(angle) + y
            dest.x - x      =       dx * cos(angle) - dy * sin(angle)
            dest.y - y      =       dy * sin(angle) + dy *
        */
        for (int i = 0; i < image.rows; ++i) {
            for (int j = 0; j < image.cols; ++j) {
                int dx = i - x;
                int dy = j - y;
                int dest_x = static_cast<int>(dx * cos(angle_rad) - dy * sin(angle_rad) + x);
                int dest_y = static_cast<int>(dx * sin(angle_rad) + dy * cos(angle_rad) + y);
                if (dest_x >= 0 && dest_x < row && dest_y >= 0 && dest_y < col) {
                    dest_data(i,j) = src_data({ dest_x, dest_y });
                }
            }
        }
        image_ = image;
    };
    if (image_.elemSize() == 1)
        rotation.operator() <uchar> ();
    else if (image_.elemSize() == 3)
        rotation.operator() <BGRPixel> ();
#endif
    return *this;
}

nl::Image& nl::Image::reverse_horizontally() {
#ifdef USE_OPENCV_LIB
    cv::flip(image_, image_, 0);
#else
    auto [data, row, col] = GetImageData<BGRPixel>(image_);

    for (int i = 0; i < row; ++i)
        for (int j = 0; j < col / 2; ++j)
            std::swap(data(i,j) , data({ i,col - j - 1 }));
#endif
    return *this;
}

// 该函数实现比cv::flip快约40%
// @TODO, 疑似未支持灰度图
nl::Image& nl::Image::reverse_vertically() {
    int row = image_.rows, col = image_.cols;
    uchar* data = image_.data;
    auto buf = new uchar[col * 3];

#define LINE(num) ((num) * col * 3)
    for (int i = 0; i < row / 2; ++i) {
        memcpy(buf,                         &data[LINE(i)],             col * 3);
        memcpy(&data[LINE(i)],              &data[LINE(row - i - 1)],   col * 3);
        memcpy(&data[LINE(row - i - 1)],    buf,                        col * 3);
    }
#undef LINE

    delete[] buf;

    return *this;
}

nl::Image& nl::Image::to_grayscale() {
#ifdef USE_OPENCV_LIB
    cv::cvtColor(image_, image_, cv::COLOR_BGR2GRAY);
#else
    auto [data, row, col] = GetImageData<BGRPixel>(image_);

    auto image = cv::Mat(row, col, CV_8UC1);
    nl::MultArray<uchar> image_data(image.data, { row,col });

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            auto pixel = data(i,j);
            int gray = pixel.R * 0.299 + pixel.G * 0.587 + pixel.B * 0.114;
            image_data(i,j) = gray;
        }
    }

    image_ = image;
#endif
    return *this;
}

nl::Image& nl::Image::to_hsv() {
#ifdef USE_OPENCV_LIB
    cv::cvtColor(image_, image_, cv::COLOR_BGR2HSV);
#else
#endif
    return *this;
}

nl::Image& nl::Image::to_binary(int threshold) {
#ifdef USE_OPENCV_LIB
    cv::threshold(image_, image_, threshold, 255, cv::THRESH_BINARY);
#else
    int elem_size = image_.elemSize();

    // @TODO 未处理RGB图像
    if (elem_size != 1)
        return *this;

    int row = image_.rows, col = image_.cols;
    nl::MultArray<uchar> data(image_.data, { row, col });


    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (data(i,j) > threshold)
                data(i,j) = 255;
            else
                data(i,j) = 0;
        }
    }
#endif
    return *this;
}

nl::Image& nl::Image::to_pseudo_color() {
#ifdef USE_OPENCV_LIB
    cv::cvtColor(image_, image_, cv::COLOR_GRAY2BGR);
#else
#endif
    return *this;
}

std::vector<std::array<size_t, 256>> nl::Image::get_histogram_data() const {
    size_t pixel_size = image_.elemSize();

    // 灰度图
    if (pixel_size == 1) {
        std::array<size_t, 256> count = { 0 };
        auto [data, row, col] = GetImageData<uchar>(image_);
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                count[data(i,j)]++;
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
                (count[B])[data(i,j).B]++;
                (count[G])[data(i,j).G]++;
                (count[R])[data(i,j).R]++;
            }
        }
        return count;
    }
    return {};
}

nl::Image nl::Image::get_histogram(int width ,int height) {
    auto arrs = get_histogram_data();

    cv::Mat ret(height,width,CV_8UC3,cv::Scalar(255,255,255));
    const double unit_width = width * 1.0 / 256;

    if (arrs.size() == 1) {
        auto arr = arrs[0];
        int max = *std::ranges::max_element(arr);

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
        return Image(ret);
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

        return Image(ret);
    }

    return Image(ret);
}

nl::Image& nl::Image::to_blur() {
    cv::GaussianBlur(image_, image_, cv::Size(3, 3), 11, 11);
    return *this;
}

nl ::Image& nl::Image::set_brightness(const int beta) {
    cv::Scalar scalar(beta,beta,beta);
    add(scalar);
    return *this;
}

nl::Image& nl::Image::set_saturation(const double alpha) {
    // @TODO 修改饱和度
    return *this;
}
nl::Image& nl::Image::set_contrast(const double alpha) {
    cv::Mat tmp_image = cv::Mat::zeros(image_.size(), image_.type());
    cv::addWeighted(image_, alpha, tmp_image, 0, 0, image_);
    return *this;
}

nl::Image &nl::Image::draw_line(cv::Point p1, cv::Point p2, const cv::Scalar& color, int pen_width) {
    cv::line(image_, p1, p2, color, pen_width, cv::LINE_8);
    return *this;
}
nl::Image &nl::Image::draw_circle(cv::Point center, int r, cv::Scalar color,int pen_width) {
    cv::circle(image_, center, r, color, pen_width, cv::LINE_8);
    return *this;
}

nl::Image &nl::Image::draw_rect(cv::Rect rect, cv::Scalar color, int pen_width = 1) {
    cv::rectangle(image_,rect,color, pen_width,cv::LINE_8,0);
    return *this;
}

nl::Image &nl::Image::draw_contours(const std::vector<cv::Point> &points, cv::Scalar color, int pen_width) {
    draw_contours(std::vector<std::vector<cv::Point>>{points},color,pen_width);
    return *this;
}
nl::Image &nl::Image::draw_contours(const std::vector<std::vector<cv::Point>> &contours, cv::Scalar color, int pen_width){
    cv::drawContours(image_,contours,-1,color,pen_width);
    return *this;
}

nl::Image &nl::Image::draw_polyline(const std::vector<cv::Point> &points, cv::Scalar color, int pen_width, bool is_closed){
    if (pen_width <= 0)
        cv::fillPoly(image_,points,color);
    else
        cv::polylines(image_, points,is_closed,color,pen_width);
    return *this;
}
nl::Image &nl::Image::draw_ellipse(cv::RotatedRect rect, cv::Scalar color, int pen_width){
    cv::ellipse(image_,rect,color,pen_width);
    return *this;
}
void nl::Image::add(const cv::Scalar &scalar) {
    cv::add(image_, scalar, image_);
}
void nl::Image::sub(const cv::Scalar &scalar) {
    cv::subtract(image_, scalar, image_);
}
void nl::Image::mul(const cv::Scalar &scalar) {
    cv::multiply(image_,scalar,image_);
}
void nl::Image::div(const cv::Scalar &scalar) {
    cv::divide(image_,scalar,image_);
}
nl::Image nl::Image::operator + (const cv::Scalar &scalar) const {
    Image tmp;
    tmp.clone_from(*this);
    tmp.add(scalar);
    return tmp;
}

nl::Image &nl::Image::operator += (const cv::Scalar &scalar) {
    add(scalar);
    return *this;
}
nl::Image nl::Image::operator - (const cv::Scalar &scalar) const {
    Image tmp;
    tmp.clone_from(*this);
    tmp.sub(scalar);
    return tmp;
}
nl::Image &nl::Image::operator -= (const cv::Scalar &scalar) {
    sub(scalar);
    return *this;
}
nl::Image nl::Image::operator * (const cv::Scalar &scalar) const {
    Image tmp;
    tmp.clone_from(*this);
    tmp.mul(scalar);
    return tmp;
}
nl::Image &nl::Image::operator *= (const cv::Scalar &scalar) {
    mul(scalar);
    return *this;
}
nl::Image nl::Image::operator / (const cv::Scalar &scalar) const {
    nl::Image tmp;
    tmp.clone_from(*this);
    tmp.div(scalar);
    return tmp;
}
nl::Image &nl::Image::operator /= (const cv::Scalar &scalar) {
    div(scalar);
    return *this;
}

nl::Image nl::Image::operator & (const nl::Image &other) const {
    Image tmp;
    tmp.clone_from(*this);
    tmp &= other;
    return tmp;
}
nl::Image nl::Image::operator | (const nl::Image &other) const {
    Image tmp;
    tmp.clone_from(*this);
    tmp |= other;
    return tmp;
}
nl::Image nl::Image::operator ~ () const {
    Image tmp;
    tmp.clone_from(*this);
    cv::bitwise_not(image_, tmp.image_);
    return tmp;
}
nl::Image nl::Image::operator ^ (const nl::Image &other) const {
    Image tmp;
    tmp.clone_from(*this);
    tmp ^= other;
    return tmp;
}

nl::Image &nl::Image::operator &= (const nl::Image &other) {
    cv::bitwise_and(image_, other.image_, image_);
    return *this;
}
nl::Image &nl::Image::operator |= (const nl::Image &other) {
    cv::bitwise_or(image_, other.image_, image_);
    return *this;
}
nl::Image &nl::Image::operator ^= (const nl::Image &other) {
    cv::bitwise_xor(image_, other.image_, image_);
    return *this;
}

nl::Image nl::Image::get_inrange(cv::Scalar lower, cv::Scalar upper) const{
    cv::Mat ret;
    cv::inRange(image_,lower,upper,ret);
    return Image(ret);
}

std::vector<nl::Image> nl::Image::split_channels() const{
    std::vector<cv::Mat> vec;
    cv::split(image_,vec);
    std::vector<Image> gray_vec{std::make_move_iterator(vec.begin()),std::make_move_iterator(vec.end())};
    return gray_vec;
}
nl::Image nl::Image::get_channel_image(int index) const {
    if (image_.channels() == 1)
        return {};

    assert(index < 3);
    std::vector<cv::Mat> vec;
    cv::split(image_,vec);
    if (index == R) {
        vec[0] = 0;
        vec[1] = 0;
    }
    else if (index == G) {
        vec[0] = 0;
        vec[2] = 0;
    }
    else if (index == B) {
        vec[1] = 0;
        vec[2] = 0;
    }
    cv::Mat ret_mat;
    cv::merge(vec,ret_mat);
    return Image(ret_mat);
}

std::vector<double> nl::Image::get_mean()const {
    cv::Mat mean, tmp;
    cv::meanStdDev(image_, mean, tmp);
    std::vector<double> ret;
    // 将平均值存储到vector中
    for (int i = 0; i < mean.total(); ++i) {
        ret.push_back(mean.at<double>(i));
    }
    return ret;
}
std::vector<double> nl::Image::get_stddev()const{
    cv::Mat stddev, tmp;
    cv::meanStdDev(image_, tmp, stddev);

    std::vector<double> ret;
    // 将平均值存储到vector中
    for (int i = 0; i < stddev.total(); ++i) {
        ret.push_back(stddev.at<double>(i));
    }
    return ret;
}
