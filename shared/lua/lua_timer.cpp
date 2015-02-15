
#include <assert.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../util/timer.h"
#include "lua_define.h"
#include "lua_config.h"
#include "lua_timer.h"

#define LUASTR_TIMERS "_timers"
#define LUASTR_REGISTERTIMER "registerTimer"
#define LUASTR_UNREGISTERTIMER "unregisterTimer"

namespace lua
{
	class Timer : public boost::enable_shared_from_this<Timer>
	{
	public:
		Timer() : _tmID(0) {}
		~Timer()
		{
			if(_tmID > 0)
			{
				lua_State* L = LuaConfig::getSingleton().getLuaState();
				assert(L != NULL);
				delTimer(L);
			}
		}

		inline void setTimerID(util::TimeID tmID){_tmID = tmID;}
		void addTimer(lua_State* L, int idx)
		{
			//register to lua table
			lua_getglobal(L, LUASTR_ENGINE);
			lua_getfield(L, -1, LUASTR_TIMERS);
			lua_pushnumber(L, _tmID);
			lua_pushvalue(L, idx);
			lua_settable(L, -3);
		}
		void delTimer(lua_State* L)
		{
			//engine._timers«Â¿Ì
			int sp = lua_gettop(L);

			lua_getglobal(L, LUASTR_ENGINE);
			lua_getfield(L, -1, LUASTR_TIMERS);
			lua_pushnumber(L, _tmID);
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
			lua_getfield(L, -1, LUASTR_TIMERS);
			lua_pushnumber(L, _tmID);
			lua_gettable(L, -2);
			luaL_checktype(L, -1, LUA_TFUNCTION);

			//pcall
			lua_pushcfunction(L, lua::error_cb);
			lua_pushvalue(L, -2);
			lua_pcall(L, 0, 0, -2);

			lua_settop(L, sp);
		}
	private:
		util::TimeID _tmID;
	};
	typedef SmartPtr<Timer> Timer_ptr;


	//lua function
	static int registerTimer(lua_State* L)
	{
		int ms = luaL_checkint(L, 1);
		luaL_checktype(L, 2, LUA_TFUNCTION);

		Timer_ptr tmPtr(new Timer());
		util::TimeID tmId = util::Timer::getSingleton().registerTimer(ms, 
			boost::bind(&Timer::handler, tmPtr->shared_from_this()));

		//std::cout << "timerID:" << tmId <<std::endl;
		if(tmId > 0)
		{
			tmPtr->setTimerID(tmId);
			tmPtr->addTimer(L, 2);
		}
		
		lua_pushnumber(L, tmId);
		return 1;
	}

	static int unregisterTimer(lua_State* L)
	{
		util::TimeID tmId = (util::TimeID)luaL_checknumber(L, 1);
		util::Timer::getSingleton().unregisterTimer(tmId);
		return 0;
	}

	//register
	void registerTimerTolua(lua_State* L)
	{
		int sp = lua_gettop(L);

		lua_getglobal(L, LUASTR_ENGINE);
		lua_pushcfunction(L, registerTimer);
		lua_setfield(L, -2, LUASTR_REGISTERTIMER);
		lua_pushcfunction(L, unregisterTimer);
		lua_setfield(L, -2, LUASTR_UNREGISTERTIMER);

		//engine._timers
		lua_newtable(L);
		lua_setfield(L, -2, LUASTR_TIMERS);

		lua_settop(L, sp);
	}
}
