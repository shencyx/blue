#define LUA_LIB
#define LUA_BUILD_AS_DLL

#include "connector.h"


extern "C" {

	LUALIB_API int luaopen_connector(lua_State *L)
	{
		connector::Connector::registToLua(L);
		return 1;
	}

}//extern c