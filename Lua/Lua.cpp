/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/11/05 22:23
*******************************************************************************/

#include "Lua.h"

#include <iostream>
Lua::Lua::Lua(std::filesystem::path path) {
    // 创建lua句柄并初始化
    L = luaL_newstate();

    if (nullptr == L) {
        std::cerr << "Lua 初始化失败" << std::endl;
        // TODO 抛出异常
    }

    luaL_openlibs(L);
    // 加载lua文件          // 执行lua文件
    if (luaL_loadfile(L, "tmp.lua") || lua_pcall(L, 0, 0, 0)) {
        std::cerr << "加载文件失败" << std::endl;
    }


    lua_getglobal(L,"str");
    std::cout << lua_tostring(L,-1) << std::endl;
    lua_getglobal(L,"number");
    std::cout << lua_tointeger(L,-1) << std::endl;


    lua_getglobal(L,"lua_table");
    lua_getfield(L,-1,"name");
    std::cout << lua_tostring(L,-1) << std::endl;

    lua_getglobal(L,"lua_table");
    lua_getfield(L,-1,"id");
    std::cout << lua_tonumber(L,-1) << std::endl;


    lua_getglobal(L,"add");
    // 参数1, 2
    lua_pushnumber(L,1);
    lua_pushnumber(L,2);
    // L,参数数量，返回值数量，错误处理程序的索引（0表示不存
    if (lua_pcall(L,2,1,0)) {
        std::cerr << lua_tostring(L,-1) << std::endl;
    }
    else {
        if (lua_isnumber(L,-1)) {
           std::cout << lua_tonumber(L,-1) << std::endl;
        }
    }



}

Lua::Lua::~Lua() {
    lua_close(L);
}
