/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/11/05 22:23
*******************************************************************************/


#pragma once

#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END(name) }

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <filesystem>

NAMESPACE_BEGIN(Lua)

class Lua {
    lua_State *L;

public:

    Lua(std::filesystem::path);

    ~Lua();

};

NAMESPACE_END(Lua)
/*
 *     lua_State* L=luaL_newstate(); luaL_newstate()函数返回一个指向堆栈的指针
 *     lua_createtable(L,0,0);新建并压入一张表
 *     lua_pushstring(L,0,0);压入一个字符串
 *     lua_pushnumber(L,0,0);压入一个数字
 *     lua_tostring(L,1);取出一个字符串
 *     lua_tointeger(L,1);取出数字
 *     double b=lua_tonumber();取出一个double类型的数字
 *     lua_load()函数 当这个函数返回0时表示加载
 *     luaL_loadfile(filename) 这个函数也是只允许加载lua程序文件，不执行lua文件。它是在内部去用lua_load()去加载指定名为filename的lua程序文件。当返回0表示没有错误。
 *     luaL_dofile 这个函数不仅仅加载了lua程序文件，还执行lua文件。返回0表示没有错误。
 *     lua_push*(L,data)压栈，
 *     lua_to*(L,index)取值，
 *     lua_pop(L,count)出栈。
 *     lua_close(L);释放lua资源
 *     lua_pcall将计算好的值压入栈顶,并返回状态值
 *     lua_getglobal(L, "val");//获取全局变量的val的值,并将其放入栈顶
 */