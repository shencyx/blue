
#include <cstdio>
#include <memory>
#include <boost/scoped_ptr.hpp>
#include "lua_mysql.h"

#define LUASTR_MYSQL "Mysql"
#define LUASTR_MYSQL_SELECTROW "selectRow"
#define LUASTR_MYSQL_SELSECTROWEX "selectRowEx"
#define LUASTR_MYSQL_SELECTALL "selectAll"
#define LUASTR_MYSQL_SELECTALLEX "selectAllEx"
#define LUASTR_MYSQL_EXECUTE "execute"
#define LUASTR_MYSQL_BEGIN "begin"
#define LUASTR_MYSQL_COMMIT "commit"
#define LUASTR_MYSQL_ROLLBACK "rollBack"
#define LUASTR_MYSQL_PING "ping"

namespace lua
{
	const char LuaMysql::className[] = LUASTR_MYSQL;
	typedef boost::scoped_ptr<mysqlpp::Statement> StatementPtr;
	typedef boost::scoped_ptr<mysqlpp::ResultSet> ResultSetPtr;

	LuaMysql::LuaMysql(lua_State* L)
	{
		const char* host = luaL_checkstring(L, 2);
		int port = luaL_checkint(L, 3);
		const char* user = luaL_checkstring(L, 4);
		const char* passwd = luaL_checkstring(L, 5);
		const char* db = luaL_checkstring(L, 6);

		lua_pushnumber(L, 0);
		lua_setfield(L, 1, "errcode");
		lua_pushstring(L, "");
		lua_setfield(L, 1, "errmsg");

		try
		{
			_conn = new mysqlpp::Connection(host, port, user, passwd, db);
			_conn->charset("UTF8");
		}
		catch(mysqlpp::MysqlError& ex)
		{
			lua_pushnumber(L, ex.error());
			lua_setfield(L, 1, "errcode");
			lua_pushstring(L, ex.what());
			lua_setfield(L, 1, "errmsg");
			_conn = NULL;

			util::ErrorLog::getSingleton().saveLogOfLevel(util::ErrorLog::LOG_LEVEL_ERROR, 
				"\n--Error Sql: %s", ex.what());
		}

	}

	LuaMysql::~LuaMysql()
	{
		if (_conn != NULL)
			delete _conn;
		_conn = NULL;
	}

	mysqlpp::Statement* LuaMysql::doSQL(lua_State* L)
	{
		lua_pushnumber(L, 0);
		lua_setfield(L, 1, "errcode");
		lua_pushstring(L, "");
		lua_setfield(L, 1, "errmsg");

		StatementPtr stmt;
		const char* sql = luaL_checkstring(L, 2);
		stmt.reset( _conn->prepare(sql) );
		int n = lua_gettop(L);
		for (int i = 3; i <= n; ++i)
		{
			if (lua_isnil(L, i)) continue;
			stmt->setString(i - 3, lua_tostring(L, i));
		}
		stmt->execute();
		//return stmt.release();
		return stmt.get();
	}

	int LuaMysql::selectRow(lua_State* L)
	{
		if (_conn == NULL)
			return 0;

		const char* sql = luaL_checkstring(L, 2);
		try
		{
			StatementPtr stmt( doSQL(L) );
			ResultSetPtr rs( stmt->resultSet() );

			if (!rs->next())
				return 0;

			//lua_newtable(L);
			lua_createtable(L, rs->fields(), 0);
			for (unsigned i = 0; i < rs->fields(); ++i)
			{
				lua_pushinteger(L, i+1);
				lua_pushstring(L, rs->getString(i).c_str());
				lua_settable(L, -3);
			}
		}
		catch(mysqlpp::MysqlError& ex)
		{
			lua_pushnumber(L, ex.error());
			lua_setfield(L, 1, "errcode");
			lua_pushstring(L, ex.what());
			lua_setfield(L, 1, "errmsg");
			lua_pushnil(L);

			util::ErrorLog::getSingleton().saveLogOfLevel(util::ErrorLog::LOG_LEVEL_ERROR, 
				"\n--Error Sql(%s): %s", sql, ex.what());
		}

		return 1;
	}

	int LuaMysql::selectRowEx(lua_State* L)
	{
		if (_conn == NULL)
			return 0;

		const char* sql = luaL_checkstring(L, 2);
		std::string sql_str = sql;
		try
		{
			StatementPtr stmt( doSQL(L) );
			ResultSetPtr rs( stmt->resultSet() );

			if (!rs->next())
				return 0;

			//lua_newtable(L);
			lua_createtable(L, 0, rs->fields());
			for (unsigned i = 0; i < rs->fields(); ++i)
			{
				lua_pushstring(L, rs->getFileNameByColNum(i).c_str());
				switch ( rs->getFieldDataType(i))
				{
				case DT_INT:
					lua_pushinteger(L, rs->getInt(i));
					break;
				case DT_LONG:
					lua_pushinteger(L, rs->getLong(i));
					break;
				case DT_DATE:
				case DT_STRING:
					lua_pushstring(L, rs->getString(i).c_str());
					break;
				case DT_FLOAT:
					lua_pushnumber(L, rs->getFloat(i));
					break;
				default:
					lua_pushnil(L);
					util::ErrorLog::getSingleton().saveLogOfLevel(util::ErrorLog::LOG_LEVEL_IMPORTANT, 
						"%s %d warning: pushnil, sql = %s", __FILE__, __LINE__, sql_str.c_str());
					break;
				}
				lua_settable(L, -3);
			}
		}
		catch(mysqlpp::MysqlError& ex)
		{
			lua_pushnumber(L, ex.error());
			lua_setfield(L, 1, "errcode");
			lua_pushstring(L, ex.what());
			lua_setfield(L, 1, "errmsg");
			lua_pushnil(L);

			util::ErrorLog::getSingleton().saveLogOfLevel(util::ErrorLog::LOG_LEVEL_ERROR, 
				"\n--Error Sql(%s): %s", sql, ex.what());
		}

		return 1;
	}


	int LuaMysql::selectAll(lua_State* L)
	{
		if (_conn == NULL)
			return 0;

		const char* sql = luaL_checkstring(L, 2);
		try
		{
			StatementPtr stmt( doSQL(L) );
			ResultSetPtr rs( stmt->resultSet() );

			lua_newtable(L);
			int rc = 1;
			while (rs->next())
			{
				lua_pushinteger(L, rc);
				//lua_newtable(L);
				lua_createtable(L, rs->fields(), 0);
				for (unsigned i = 0; i < rs->fields(); ++i)
				{
					lua_pushinteger(L, i+1);
					lua_pushstring(L, rs->getString(i).c_str());
					lua_settable(L, -3);
				}
				lua_settable(L, -3);
				++rc;
			}
		}
		catch(mysqlpp::MysqlError& ex)
		{
			lua_pushnumber(L, ex.error());
			lua_setfield(L, 1, "errcode");
			lua_pushstring(L, ex.what());
			lua_setfield(L, 1, "errmsg");
			lua_pushnil(L);

			util::ErrorLog::getSingleton().saveLogOfLevel(util::ErrorLog::LOG_LEVEL_ERROR, 
				"\n--Error Sql(%s): errmsg:%s, errcode:%d", sql, ex.what(), ex.error());
		}

		return 1;
	}

	int LuaMysql::selectAllEx(lua_State* L)
	{
		if (_conn == NULL)
			return 0;

		const char* sql = luaL_checkstring(L, 2);
		std::string sql_str = sql;
		try
		{
			StatementPtr stmt( doSQL(L) );
			ResultSetPtr rs( stmt->resultSet() );

			lua_newtable(L);
			int rc = 1;
			while (rs->next())
			{
				lua_pushinteger(L, rc);
				//lua_newtable(L);
				lua_createtable(L, 0, rs->fields());
				for (unsigned i = 0; i < rs->fields(); ++i)
				{
					lua_pushstring(L, rs->getFileNameByColNum(i).c_str());
					switch ( rs->getFieldDataType(i))
					{
					case DT_INT:
						lua_pushinteger(L, rs->getInt(i));
						break;
					case DT_LONG:
						lua_pushinteger(L, rs->getLong(i));
						break;
					case DT_DATE:
					case DT_STRING:
						lua_pushstring(L, rs->getString(i).c_str());
						break;
					case DT_FLOAT:
						lua_pushnumber(L, rs->getFloat(i));
						break;
					default:
						lua_pushnil(L);
						util::ErrorLog::getSingleton().saveLogOfLevel(util::ErrorLog::LOG_LEVEL_IMPORTANT, 
							"%s %d warning: pushnil, sql = %s", __FILE__, __LINE__, sql_str.c_str());
						break;
					}
					lua_settable(L, -3);
				}
				lua_settable(L, -3);
				++rc;
			}
		}
		catch(mysqlpp::MysqlError& ex)
		{
			lua_pushnumber(L, ex.error());
			lua_setfield(L, 1, "errcode");
			lua_pushstring(L, ex.what());
			lua_setfield(L, 1, "errmsg");
			lua_pushnil(L);

			util::ErrorLog::getSingleton().saveLogOfLevel(util::ErrorLog::LOG_LEVEL_ERROR, 
				"\n--Error Sql(%s): %s", sql, ex.what());
		}

		return 1;
	}

	int LuaMysql::execute(lua_State* L)
	{
		if (_conn == NULL)
			return 0;

		const char* sql = luaL_checkstring(L, 2);
		try
		{
			StatementPtr stmt( doSQL(L) );
			lua_pushinteger(L, stmt->affectedRows());
		}
		catch(mysqlpp::MysqlError& ex)
		{
			lua_pushnumber(L, ex.error());
			lua_setfield(L, 1, "errcode");
			lua_pushstring(L, ex.what());
			lua_setfield(L, 1, "errmsg");
			lua_pushnil(L);

			util::ErrorLog::getSingleton().saveLogOfLevel(util::ErrorLog::LOG_LEVEL_ERROR, 
				"\n--Error Sql(%s): %s", sql, ex.what());
		}

		return 1;
	}

	//ÊÂÎñ
	int LuaMysql::begin(lua_State* L)
	{
		return transaction(L, 1);
	}
	int LuaMysql::commit(lua_State* L)
	{
		return transaction(L, 2);
	}
	int LuaMysql::rollBack(lua_State* L)
	{
		return transaction(L, 3);
	}

	int LuaMysql::transaction(lua_State* L, int flag)
	{
		if (_conn == NULL)
			return 0;

		try
		{
			if(flag == 1)
				_conn->begin();
			else if(flag == 2)
				_conn->commit();
			else if(flag == 3)
				_conn->rollBack();
		}
		catch(mysqlpp::MysqlError& ex)
		{
			lua_pushnumber(L, ex.error());
			lua_setfield(L, 1, "errcode");
			lua_pushstring(L, ex.what());
			lua_setfield(L, 1, "errmsg");
			lua_pushnil(L);

			util::ErrorLog::getSingleton().saveLogOfLevel(util::ErrorLog::LOG_LEVEL_ERROR, 
				"\n--Error Sql: %s", ex.what());
		}

		return 0;
	}


	int LuaMysql::ping(lua_State* L)
	{
		if (_conn == NULL)
			return 0;
		lua_pushboolean(L, (bool)(*_conn));
		return 1;
	}

	void LuaMysql::registerToLua(lua_State *L)
	{
		lua::LuaClass<LuaMysql> lc(L);
		lc.def<&LuaMysql::selectRow>(LUASTR_MYSQL_SELECTROW);
		lc.def<&LuaMysql::selectRowEx>(LUASTR_MYSQL_SELSECTROWEX);
		lc.def<&LuaMysql::selectAll>(LUASTR_MYSQL_SELECTALL);
		lc.def<&LuaMysql::selectAllEx>(LUASTR_MYSQL_SELECTALLEX);
		lc.def<&LuaMysql::execute>(LUASTR_MYSQL_EXECUTE);

		lc.def<&LuaMysql::begin>(LUASTR_MYSQL_BEGIN);
		lc.def<&LuaMysql::commit>(LUASTR_MYSQL_COMMIT);
		lc.def<&LuaMysql::rollBack>(LUASTR_MYSQL_ROLLBACK);

		lc.def<&LuaMysql::ping>(LUASTR_MYSQL_PING);
	}
}
