/*********************************************************************
 * Auther : yongheng
 * Date : 2022/10/14 20:02
*********************************************************************/
#ifndef LINUXCOMMAND_MYPWD_CPP_H
#define LINUXCOMMAND_MYPWD_CPP_H
#include <string>
#include <filesystem>

std::string MyPwdCpp17(){
    std::filesystem::path p = std::filesystem::current_path();
    return p.string();
}
#endif //LINUXCOMMAND_MYPWD_CPP_H
