#define LUA_LIB
#define LUA_BUILD_AS_DLL

#include <stdlib.h>
#include <time.h> 
#include <stdio.h> 
#include "lua.hpp"
#include "log.h"
#include "shared/util/SharedPtr.h"

using namespace logS;

static int lua_add(lua_State *L)
{
	double a = luaL_checknumber(L, 1);
	double b = luaL_checknumber(L, 2);

	lua_pushnumber(L, a + b);

	return 1;
}


static int lua_hello(lua_State *L)
{
	char tmp[] = "hello blue!";
	lua_pushstring(L, tmp);

	return 1;
}

static int lua_new(lua_State *L)
{
	const char * pPath= luaL_checkstring(L, 1);
	Log *p = new Log(L, "name");


	lua_pushlightuserdata(L, (void*)p);

	return 1;
}

static const struct luaL_Reg log_reg[] = 
{
	{"add",			lua_add},
	{"hello",		lua_hello},
	{"new",		lua_new},
	{NULL,		NULL}
};

static const struct luaL_Reg log_new[] = 
{
	{"new",		lua_new},
	{NULL,		NULL}
};

extern "C" {

	LUALIB_API int luaopen_log(lua_State *L)
	{
		printf("luaopen_log\n");
		/*
		luaL_newmetatable(L, "log_service");
		luaL_register(L, "log", log_reg);
		return 1;
		*/
		Log::registToLua(L);
		return 1;
	}

}//extern c
