/*
* ic = iconv:New(from_charset, to_charset)
* sz = ic(sz)
*/
#pragma once
#include <string>
#include "../util/config.h"
#include "../util/lua_class.h"

#if defined(LINUX)
#include <iconv.h>
#endif

namespace lua
{

	class LuaIconv 
	{
	public:
		LuaIconv(lua_State* L);
		~LuaIconv();
		int setup(lua_State* L);
		int conv(lua_State* L);
	private:
#if defined(LINUX)
		iconv_t _ic;
#else
		std::string _from;
#endif
	public:
		static const char className[];
		static void registerToLua(lua_State* L);
	};
}
