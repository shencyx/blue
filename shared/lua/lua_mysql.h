/*
* dbh = MySQL:New(host, port, db, user, passwd)
* t = dbh:selectrow(sql, param1, param2, ..)
* t = dbh:selectall(sql, param1, param2, ..)
* dbh:execute(sql, param1, param2, ..)
*/
#pragma once
#include "../util/config.h"
#include "../util/lua_class.h"
#include "../db/mysqlpp.h"

namespace lua
{
	class LuaMysql 
	{
	public:
		static const char className[];
		static void registerToLua(lua_State* L);
	public:
		LuaMysql(lua_State* L);
		~LuaMysql();
		int selectRow(lua_State* L);
		int selectRowEx(lua_State* L);
		int selectAll(lua_State* L);
		int selectAllEx(lua_State* L);
		int execute(lua_State* L);

		int begin(lua_State* L);
		int commit(lua_State* L);
		int rollBack(lua_State* L);

		int ping(lua_State* L);
	private:
		mysqlpp::Statement* doSQL(lua_State* L);
		int transaction(lua_State* L, int flag);
	private:
		mysqlpp::Connection* _conn;
	};
}

