#define LUA_LIB
#define LUA_BUILD_AS_DLL

#include "logic.h"


extern "C" {

	LUALIB_API int luaopen_logic(lua_State *L)
	{
		//luaL_register(L, "blue", blue_reg);
		logic::Logic::registToLua(L);
		return 1;
	}

}//extern c