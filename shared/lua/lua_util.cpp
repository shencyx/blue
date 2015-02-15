
#include "../util/config.h"
#include "../util/error_log.h"
#include "../rpc/protocol.h"

#include "lua_define.h"
#include "lua_util.h"

#define LUASTR_UTIL_NET_INDEX_INVALID "NET_INDEX_INVALID"
#define LUASTR_UTIL_WRITELOG "writeLog"
#define LUASTR_UTIL_PRINTMESSAGEDECODE "printMessageDecode"
#define LUASTR_UTIL_PRINTMESSAGEENCODE "printMessageEncode"

namespace lua
{
	void LuaUtil::registerToLua(lua_State* L)
	{
		lua_getglobal(L, LUASTR_ENGINE);

		lua_pushinteger(L, NET_INDEX_INVALID);
		lua_setglobal(L, LUASTR_UTIL_NET_INDEX_INVALID);

		lua_pushcfunction(L, &LuaUtil::writeLog);
		lua_setfield(L, -2, LUASTR_UTIL_WRITELOG);

		lua_pushcfunction(L, &LuaUtil::printMessageDecode);
		lua_setfield(L, -2, LUASTR_UTIL_PRINTMESSAGEDECODE);

		lua_pushcfunction(L, &LuaUtil::printMessageEncode);
		lua_setfield(L, -2, LUASTR_UTIL_PRINTMESSAGEENCODE);
	}

	int LuaUtil::writeLog(lua_State* L)
	{
		const char* str = luaL_checkstring(L, 1);
		util::ErrorLog::getSingleton().saveLog(str);
		return 0;
	}

	int LuaUtil::printMessageDecode(lua_State* L)
	{
		NET_NUMBER numNet = luaL_checkint(L, 1);
		rpc::glproto::printMessageDecode(numNet, NULL);
		return 0;
	}

	int LuaUtil::printMessageEncode(lua_State* L)
	{
		NET_NUMBER numNet = luaL_checkint(L, 1);
		rpc::glproto::printMessageEncode(numNet, NULL);
		return 0;
	}
}