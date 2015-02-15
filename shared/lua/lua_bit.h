
#pragma once
#include "../util/lua_class.h"

namespace lua
{
	class LuaBit
	{
	public:
		static void registerToLua(lua_State* L);
	private:
		LuaBit(lua_State* L);
		
		static int Or(lua_State* L);
		static int And(lua_State* L);
		static int Xor(lua_State* L);
		static int Not(lua_State* L);
		static int LShift(lua_State* L);
		static int RShift(lua_State* L);
	};
}
