
#include <assert.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../util/signal.h"
#include "lua_define.h"
#include "lua_config.h"
#include "lua_signal.h"

#define LUASTR_SIGNAL "_signal"
#define LUASTR_REGISTERSIGNAL "registerSignal"

namespace lua
{
	class Signal : public boost::enable_shared_from_this<Signal>
	{
	public:
		Signal(int sig) : _sig(sig) {}
		~Signal()
		{
			lua_State* L = LuaConfig::getSingleton().getLuaState();
			assert(L != NULL);
			delSignal(L);
		}

		void addSignal(lua_State* L, int idx)
		{
			//register to lua table
			lua_getglobal(L, LUASTR_ENGINE);
			lua_getfield(L, -1, LUASTR_SIGNAL);
			lua_pushnumber(L, _sig);
			lua_pushvalue(L, idx);
			lua_settable(L, -3);
		}
		void delSignal(lua_State* L)
		{
			//engine._signal«Â¿Ì
			int sp = lua_gettop(L);

			lua_getglobal(L, LUASTR_ENGINE);
			lua_getfield(L, -1, LUASTR_SIGNAL);
			lua_pushnumber(L, _sig);
			lua_pushnil(L);
			lua_settable(L, -3);

			lua_settop(L, sp);
		}

		void handler()
		{
			lua_State* L = LuaConfig::getSingleton().getLuaState();
			assert(L != NULL);

			int sp = lua_gettop(L);

			lua_getglobal(L, LUASTR_ENGINE);
			lua_getfield(L, -1, LUASTR_SIGNAL);
			lua_pushinteger(L, _sig);
			lua_gettable(L, -2);
			luaL_checktype(L, -1, LUA_TFUNCTION);

			//pcall
			lua_pushcfunction(L, lua::error_cb);
			lua_pushvalue(L, -2);
			lua_pushinteger(L, _sig);
			lua_pcall(L, 1, 0, -3);

			lua_settop(L, sp);
		}
	private:
		int _sig;
	};
	typedef SmartPtr<Signal> Signal_ptr;


	//lua function
	static int registerSignal(lua_State* L)
	{
		int sig = luaL_checkint(L, 1);
		luaL_checktype(L, 2, LUA_TFUNCTION);

		Signal_ptr sigPtr(new Signal(sig));
		if(util::Signal::getSingleton().registerSignal(sig, 
			boost::bind(&Signal::handler, sigPtr->shared_from_this())))
		{
			sigPtr->addSignal(L, 2);
		}

		return 0;
	}

	//register
	void registerSignalTolua(lua_State* L)
	{
		int sp = lua_gettop(L);

		lua_getglobal(L, LUASTR_ENGINE);
		lua_pushcfunction(L, registerSignal);
		lua_setfield(L, -2, LUASTR_REGISTERSIGNAL);

		//engine._signal
		lua_newtable(L);
		lua_setfield(L, -2, LUASTR_SIGNAL);

		lua_settop(L, sp);
	}
}
