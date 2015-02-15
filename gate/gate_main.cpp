#define LUA_LIB
#define LUA_BUILD_AS_DLL

#include "gate.h"


extern "C" {

	LUALIB_API int luaopen_gate(lua_State *L)
	{
		//luaL_register(L, "blue", blue_reg);
		gate::Gate::registToLua(L);
		return 1;
	}

}//extern c