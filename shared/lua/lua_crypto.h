/*
 * md5sum = crypto.md5(s1, s2, s3)
 * r = crypto.random()  -- same args as math.random
 * id = crypto.uuid()
 */
#pragma once
#include "../util/lua_class.h"

namespace lua
{
	class LuaCrypto
	{
	public:
		static void registerToLua(lua_State* L);
	private:
		LuaCrypto(lua_State *L);

		static int md5(lua_State* L);
		static int random(lua_State* L);
		static int uuid(lua_State* L);
		static int localTime(lua_State* L);
		static int localTimeEx(lua_State* L);
		static int time(lua_State* L);
		static int desDecrypt(lua_State* L);
	};
}
