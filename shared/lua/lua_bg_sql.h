
#pragma once
#include "../util/lua_class.h"

namespace lua
{
	class LuaBGSql
	{
	public:
		LuaBGSql(lua_State* L);
		~LuaBGSql();
	public:
		static const char className[];
		static void registerToLua(lua_State* L);

		//int start(lua_State* L);
		//int stop(lua_State* L);
		int write(lua_State* L);
	private:
		//db::BGSql _sql;
	};
}

