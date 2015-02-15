#define LUA_LIB
#define LUA_BUILD_AS_DLL

#include "lua.hpp"
#include "json/lua_cjson.h"
#include "bson/bson.h"
#include "serialize/serialize.h"
#include "lxp/lxplib.h"

static const struct luaL_Reg util_reg[] = 
{
	{NULL,	NULL}
};


extern "C" {

	LUALIB_API int luaopen_util(lua_State *L)
	{
		//printf("luaopen_util\n");
		//luaL_register(L, "util", util_reg);
		lua::luaopen_cjson(L);
		luaopen_bson(L);
		luaopen_serialize(L);
		luaopen_lxp(L);
		//lua_newtable(L);
		return 0;
	}

}//extern c
