/*********************************************************************
 * Auther : yongheng
 * Date : 2022/10/14 19:23
*********************************************************************/
#ifndef LINUXCOMMAND_MYLS_CPP_H
#define LINUXCOMMAND_MYLS_CPP_H

#include <filesystem>
#include <string>

std::string MyLsCpp17(){

    std::filesystem::path p = std::filesystem::current_path();
    std::string curpwd;
    for(auto it : std::filesystem::directory_iterator(p)){
        curpwd += it.path().filename();
        curpwd += "\t";
    }

    return curpwd;
}








#endif //LINUXCOMMAND_MYLS_CPP_H
