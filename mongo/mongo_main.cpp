#define LUA_LIB
#define LUA_BUILD_AS_DLL

#include "mongo.h"


extern "C" {

	LUALIB_API int luaopen_mongo(lua_State *L)
	{
		//luaL_register(L, "blue", blue_reg);
		mongo::Mongo::registToLua(L);
		return 1;
	}

}//extern c