
#pragma once
#include "../util/lua_class.h"
#include "../util/net_info.h"
#include "../util/idmap.h"

namespace lua
{
	class LuaUtil
	{
	public:
		static void registerToLua(lua_State* L);
	private:
		LuaUtil(lua_State *L);

		//log
		static int writeLog(lua_State* L);

		//print protocol
		static int printMessageDecode(lua_State* L);
		static int printMessageEncode(lua_State* L);
	private:
	};
}
