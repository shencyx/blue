 #define LUA_LIB
 #define LUA_BUILD_AS_DLL

#include <stdlib.h>
#include <time.h> 
#include <stdio.h> 
#include "lua.hpp"
#include "shared/server.h"

#include "shared/util/Timer.h"

/*
static int lua_add(lua_State *L)
{
	double a = luaL_checknumber(L, 1);
	double b = luaL_checknumber(L, 2);

	lua_pushnumber(L, a + b);

	return 1;
}

static int lua_time(lua_State *L)
{
	time_t t = time(0); 
	char tmp[64]; 
	strftime(tmp, sizeof(tmp), "%Y/%m/%d %X %A 本年第%j天 %z", localtime(&t));

	lua_pushstring(L, tmp);

	return 1;
}

static int lua_hello(lua_State *L)
{
	char tmp[] = "hello blue!";
	lua_pushstring(L, tmp);

	return 1;
}

static int lua_getTime(lua_State *L)
{
	uint32 t = Timer::getTime();
	uint64 t2 = Timer::getTimeMs();

	printf("lua_getTime:%d,%llu\n", t, t2);
	lua_pushnumber(L, t);
	lua_pushnumber(L, t2);

	return 2;
}

static int lua_setf(lua_State *L)
{
	int f = (int)luaL_checknumber(L, 1);
	
	sv.setFrameTime(f);

	return 0;
}

static int lua_start(lua_State *L)
{
	printf("sv p:%p\n", &sv);
	sv.start();

	return 0;
}

static int registService(lua_State *L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_getfield(L, 1, "__cobj");
	void ** pService = (void**)lua_touserdata(L, -1);
	ASSERT(*pService != NULL);
	const char * name = luaL_checkstring(L, 2);
	sv.registService((shared::Service *)*pService, name);

	return 0;
}

static const struct luaL_Reg blue_reg[] = 
{
	{"add",			lua_add},
	{"hello",		lua_hello},
	{"getTime",		lua_getTime},
	{"start",		lua_start},
	{"registService",		registService},
	{"lua_setf",		lua_setf},
	{NULL,		NULL}
};
*/

extern "C" {

LUALIB_API int luaopen_blue(lua_State *L)
{
	//luaL_register(L, "blue", blue_reg);
	shared::Server::registToLua(L);
	return 1;
}

}//extern c
