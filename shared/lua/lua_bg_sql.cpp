
#include "../db/bg_sql.h"
#include "lua_bg_sql.h"

#define LUASTR_BGSQL "BGSql"
#define LUASTR_BGSQL_WRITE "write"

namespace lua
{
	const char LuaBGSql::className[] = LUASTR_BGSQL;

	LuaBGSql::LuaBGSql(lua_State* L)
	{
	}

	LuaBGSql::~LuaBGSql()
	{
	}

	/*int LuaBGSql::start(lua_State* L)
	{
		const char* host = luaL_checkstring(L, 2);
		int port = luaL_checkint(L, 3);
		const char* user = luaL_checkstring(L, 4);
		const char* passwd = luaL_checkstring(L, 5);
		const char* db = luaL_checkstring(L, 6);

		int ret = _sql.start(host, port, user, passwd, db);
		lua_pushinteger(L, ret);
		return 1;
	}
	int LuaBGSql::stop(lua_State* L)
	{
		_sql.stop();
		return 0;
	}*/

	int LuaBGSql::write(lua_State* L)
	{
		const char* name = luaL_checkstring(L, 2);
		db::BGSql::getSingleton().write(name);
		//lua_pushinteger(L, ret);
		return 0;
	}

	void LuaBGSql::registerToLua(lua_State* L)
	{
		lua::LuaClass<LuaBGSql> lc(L);
		//lc.def<&LuaBGSql::start>("start");
		//lc.def<&LuaBGSql::stop>("stop");
		lc.def<&LuaBGSql::write>(LUASTR_BGSQL_WRITE);
	}
}

