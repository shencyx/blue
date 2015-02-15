
#include "lua_define.h"
#include "lua_bit.h"

#define LUASTR_BIT_AND "And"
#define LUASTR_BIT_OR "Or"
#define LUASTR_BIT_NOT "Not"
#define LUASTR_BIT_XOR "Xor"
#define LUASTR_BIT_LSHIFT "LShift"
#define LUASTR_BIT_RSHIFT "RShift"

namespace lua
{
	void LuaBit::registerToLua(lua_State *L)
	{
		lua_getglobal(L, LUASTR_ENGINE);
	
		lua_pushcfunction(L, &LuaBit::And);
		lua_setfield(L, -2, LUASTR_BIT_AND);

		lua_pushcfunction(L, &LuaBit::Or);
		lua_setfield(L, -2, LUASTR_BIT_OR);

		lua_pushcfunction(L, &LuaBit::Not);
		lua_setfield(L, -2, LUASTR_BIT_NOT);

		lua_pushcfunction(L, &LuaBit::Xor);
		lua_setfield(L, -2, LUASTR_BIT_XOR);

		lua_pushcfunction(L, &LuaBit::LShift);
		lua_setfield(L, -2, LUASTR_BIT_LSHIFT);

		lua_pushcfunction(L, &LuaBit::RShift);
		lua_setfield(L, -2, LUASTR_BIT_RSHIFT);
	}

	int LuaBit::And(lua_State* L)
	{
		int num1 = luaL_checkint(L, 1);
		int num2 = luaL_checkint(L, 2);
		int result = num1 & num2;
		lua_pushinteger (L, result);
		return 1;
	}

	int LuaBit::Or(lua_State* L)
	{
		int num1 = luaL_checkint(L, 1);
		int num2 = luaL_checkint(L, 2);
		int result = num1 | num2;
		lua_pushinteger (L, result);
		return 1;
	}

	int LuaBit::Not(lua_State* L)
	{
		int num1 = luaL_checkint(L, 1);
		int result = ~num1;
		lua_pushinteger (L, result);
		return 1;
	}

	int LuaBit::Xor(lua_State* L)
	{
		int num1 = luaL_checkint(L, 1);
		int num2 = luaL_checkint(L, 2);
		int result = num1 ^ num2;
		lua_pushinteger (L, result);
		return 1;
	}

	int LuaBit::LShift(lua_State* L)
	{
		int num1 = luaL_checkint(L, 1);
		int num2 = luaL_checkint(L, 2);
		int result = num1 << num2;
		lua_pushinteger (L, result);
		return 1;
	}

	int LuaBit::RShift(lua_State* L)
	{
		int num1 = luaL_checkint(L, 1);
		int num2 = luaL_checkint(L, 2);
		int result = num1 >> num2;
		lua_pushinteger (L, result);
		return 1;
	}
}
