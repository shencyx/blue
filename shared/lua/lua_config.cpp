
#include <assert.h>

#include "lua_define.h"
#include "lxplib.h"
#include "lua_timer.h"
#include "lua_signal.h"
#include "mongodb/lua_MongoDB.h"
#include "rpc/lua_rpc.h"
#include "rpc/lua_rpcbuffer.h"
#include "lua_mysql.h"
#include "lua_bg_sql.h"
#include "json/lua_cjson.h"
#include "lua_bit.h"
#include "lua_crypto.h"
#include "lua_iconv.h"
#include "lua_util.h"
#include "lua_config.h"

#define LUASTR_JSON "json"

namespace lua
{
	LuaConfig::LuaConfig() : _L(NULL), _flag(false)
	{

	}

	LuaConfig::~LuaConfig()
	{

	}

	void LuaConfig::openLua(lua_State* L)
	{
		assert(L != NULL);
		_L = L;
		_flag = true;
	}

	void LuaConfig::closeLua()
	{
		_L = NULL;
		_flag = false;
	}

	void LuaConfig::utilRegisterTolua()
	{
		if(!isFlag()) return;
		lua_State* L = getLuaState();
		assert(L != NULL);

		int sp = lua_gettop(L);
		//xml
		luaopen_lxp(L);
		//timer
		registerTimerTolua(L);
		//signal
		registerSignalTolua(L);
		//rpc
		luarpc::registerRpcToLua(L);
		luarpc::LuaRpcBuffer::registerToLua(L);
		//mysql
		LuaMysql::registerToLua(L);
		//bgsql
		LuaBGSql::registerToLua(L);
		//mongodb
		LuaMongoDB::registerToLua(L);
		//json
		lua_getglobal(L, LUASTR_ENGINE);
		lua_pushstring(L, LUASTR_JSON);
		luaopen_cjson(L);
		lua_settable(L, -3);
		//bit
		LuaBit::registerToLua(L);
		//crypto
		LuaCrypto::registerToLua(L);
		//iconv
		LuaIconv::registerToLua(L);
		//util
		LuaUtil::registerToLua(L);
		lua_settop(L, sp);
	}
}
