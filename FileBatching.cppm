/*******************************************************************************
 * Author  : yongheng
 * Data    : 2024/5/18 0:8
 * warning : 使用vs时，由于wstring_convert 和convert_utf8已经已经被C++17标记为弃用
 *           因此需要添加宏：_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
*******************************************************************************/
module;
#include <algorithm>
#include <iterator>
#include <string>
#include <codecvt>
#include <fstream>
#include <filesystem>
#include <functional>
export module FileBatching;


#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END(name) }

#if defined(_WIN32) || defined(_WIN64)
const char* const GbkLocalString = "zh_CN";
#else
const char* const GbkLocalString = "zh_CN.GBK";
#endif


bool DetectUtf8Coding(const std::string_view text) {

    unsigned int byte_count = 0;//UFT8可用1-6个字节编码,ASCII用一个字节

    bool all_ascii = true;
    for (const unsigned char ch : text) {
        //判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
        if (byte_count == 0 && (ch & 0x80) != 0) {
            all_ascii = false;
        }
        if (byte_count == 0) {
            //如果不是ASCII码,应该是多字节符,计算字节数
            if (ch >= 0x80) {
                if (ch >= 0xFC && ch <= 0xFD) {
                    byte_count = 6;
                }
                else if (ch >= 0xF8) {
                    byte_count = 5;
                }
                else if (ch >= 0xF0) {
                    byte_count = 4;
                }
                else if (ch >= 0xE0) {
                    byte_count = 3;
                }
                else if (ch >= 0xC0) {
                    byte_count = 2;
                }
                else {
                    return false;
                }
                byte_count--;
            }
        }
        else {
            //多字节符的非首字节,应为 10xxxxxx
            if ((ch & 0xC0) != 0x80) {
                return false;
            }
            //减到为零为止
            byte_count--;
        }

    }
    //违返UTF8编码规则
    if (byte_count != 0) {
        return false;
    }
    if (all_ascii) { //如果全部都是ASCII, 也是UTF8
        return true;
    }
    return true;
}

bool DetectGBKCoding(const std::string_view text) {
    unsigned int byte_count = 0;//GBK可用1-2个字节编码,中文两个 ,英文一个
    bool all_ascii = true; //如果全部都是ASCII,
    for (const unsigned char ch : text) {
        if (( ch & 0x80) != 0 && byte_count == 0) {// 判断是否ASCII编码,如果不是,说明有可能是GBK
            all_ascii = false;
        }
        if (byte_count == 0) {
            if ( ch >= 0x80) {
                if ( ch >= 0x81 &&  ch <= 0xFE) {
                    byte_count = +2;
                }
                else {
                    return false;
                }
                byte_count--;
            }
        }
        else {
            if ( ch < 0x40 ||  ch>0xFE) {
                return false;
            }
            byte_count--;
        }//else end

    }
    if (byte_count != 0) {   //违返规则
        return false;
    }
    if (all_ascii) { //如果全部都是ASCII, 也是GBK
        return true;
    }
    return true;
}


class codecvt_gbk final : public std::codecvt_byname<wchar_t, char, std::mbstate_t>
{
public:
    codecvt_gbk() : codecvt_byname(GbkLocalString) {

    }
};


static std::wstring_convert<codecvt_gbk> gbk_convert;
static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;



export NAMESPACE_BEGIN(FileBatching)

/*
* @brief : 获取相对路径层数
* @param root_path : 根路径
* @param cur_path : 当前路径
* @return : 当前路径相对于根路径的层数
*/
auto GetDeep(const std::filesystem::path& root_path, const std::filesystem::path& cur_path) {
    auto path = cur_path.lexically_relative(root_path).generic_string();
    return std::ranges::count(path.begin(), path.end(), '/') + 1;
}

enum class Encoding {
    GBK,UTF8,ERROR
};
std::ostream & operator << (std::ostream &out, Encoding encoding) {
    switch (encoding) {
    case Encoding::GBK :
        out << "GBK";
        break;
    case Encoding::UTF8 :
        out << "UTF8";
        break;
    case Encoding::ERROR :
        out << "Error";
        break;
    default :
        break;
    }
    return out;
}

/**
* @brief : 检测文件的编码格式
* @param file_path : 文件路径
* @return : 文件编码格式，仅支持 UTF-8和GBK, 其他编码类型返回ERROR
*/
Encoding DetectEncoding(const std::filesystem::path& file_path) {
    std::ifstream file(file_path);
    std::string context((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (DetectUtf8Coding(context) == true) {
        return Encoding::UTF8;
    }
    if (DetectGBKCoding(context) == true) {
        return Encoding::GBK;
    }
    return Encoding::ERROR;
}

std::wstring GbkToUnicode(const std::string& str) {
    return gbk_convert.from_bytes(str);
}
std::string UnicodeToGbk(const std::wstring& str) {
    return gbk_convert.to_bytes(str);
}
std::wstring Utf8ToUnicode(const std::string& str) {
    return gbk_convert.from_bytes(str);
}
std::string UnicodeToUtf8(const std::wstring& str) {
    return gbk_convert.to_bytes(str);
}

// read_to_string
// read_to_wstring
std::wstring ReadToWString(const std::filesystem::path &file_path) {
    std::wifstream in(file_path);
    in.imbue(std::locale(GbkLocalString));
    return {std::wstring(std::istreambuf_iterator<wchar_t>(in),std::istreambuf_iterator<wchar_t>())};
}
std::string ReadToString(const std::filesystem::path &file_path) {
    std::ifstream in(file_path);
    return {std::string(std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>())};
}

// save_string_to_file
// save_wstring_to_file
void SaveToFile(const std::wstring &str,const std::filesystem::path &file_path) {
    std::wofstream out_file(file_path);
    out_file.imbue(std::locale(GbkLocalString));
    out_file << str;
}
void SaveToFile(const std::string &str,const std::filesystem::path &file_path) {
    std::ofstream out_file(file_path);
    out_file << str;
}


// wstring_to_string
// string_to_wstring
std::string WStringToString(const std::wstring &str) {
    // 将宽字符串转换为窄字符串（GBK编码）
    return converter.to_bytes(str);
}
std::wstring StringToWString(const std::string &str) {
    return converter.from_bytes(str);
}


// string_gbk_to_utf8
// wstring_gbk_to_utf8
std::string GbkToUtf8(const std::wstring &str) {
    return WStringToString(str);
}
std::string GbkToUtf8(const std::string& str) {
    return converter.to_bytes(gbk_convert.from_bytes(str));
}

// utf8_to_string_gbk
// utf8_to_wstring_gbk
std::wstring Utf8ToWstringGbk(const std::string &str) {
    return StringToWString(str);
}
std::string Utf8ToStringGbk(const std::string& str) {
    return gbk_convert.to_bytes(converter.from_bytes(str));
}
class FileBatching {
    std::string root_path_ = ".";
    std::function<void(std::filesystem::path)> batchint_func_;

    [[nodiscard]]
    auto GetDeep(const std::filesystem::path& cur_path) const {
        namespace fb = FileBatching;
        return fb::GetDeep(root_path_, cur_path);
    }

public:
    bool no_batching_directory = false;
    size_t deep = 0;
    size_t limit_times = 0;


    template <typename Func>
    FileBatching(std::string &&root_path,Func func)
            : root_path_(root_path) ,batchint_func_(func) {

    }

    void operator() () const {
        for (size_t times = 0 ;const auto& file : std::filesystem::recursive_directory_iterator(root_path_)) {

            if (deep && GetDeep(file) > deep)
                continue;
            if (no_batching_directory && std::filesystem::is_directory(file))
                continue;

            batchint_func_(file);

            if (limit_times && ++times >= limit_times)
                return;
        }
    }

};

NAMESPACE_END(FileBatching)


