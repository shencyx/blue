#include "logic.h"
#include "shared/Server.h"
#include "shared/util/Timer.h"
#include "shared/GlobalDefine.h"

namespace logic
{
	const char Logic::serviceName[] = "logic_service";
	const static char HANDLE_SERVER_JSON[] = "handles_server_json";
	const static char HANDLE_SERVER_SERI[] = "handles_server_seri";
	const static char HANDLE_CLIENT_JSON[] = "handles_client_json";
	const static char HANDLE_CLIENT_PB[] = "handles_client_pb";

	Logic::Logic(lua_State * L, const char * pName):
		Service(L, pName),
		_frameTime(1000)
	{
		_logicState = luaL_newstate();
		luaL_openlibs(_logicState);
		initMy();
	}

	Logic::~Logic()
	{
		lua_close(_logicState);
	}

	void Logic::initMy()
	{
		lua_pushlightuserdata(_logicState, (void*)this);
		lua_setglobal(_logicState, "my");
		//
		luaL_newmetatable(_logicState, "my");
		// __index
		lua_newtable(_logicState);
		lua_pushcfunction(_logicState, Logic::sendInsLuaMy);
		lua_setfield(_logicState, -2, "sendIns");
		lua_pushcfunction(_logicState, Logic::getTimeMsMy);
		lua_setfield(_logicState, -2, "getNowMs");

		//
		lua_setfield(_logicState, -2, "__index");
		lua_getglobal(_logicState, "my");
		lua_pushvalue(_logicState, -2);
		lua_setmetatable(_logicState, -2);
	}

	bool Logic::onTimer(uint64 now)
	{
		//printf("Logic::onTimer(%lld)\n", now);
		int top = lua_gettop(_logicState);
		lua_getglobal(_logicState, "logicTimer");
		if (LUA_TFUNCTION == lua_type(_logicState, -1))
		{
			lua_pushnumber(_logicState, (lua_Number)now);
			//lua_pushstring(_logicState, "what");
			int ret = lua_pcall(_logicState, 1, 0, 0);
			if (ret != 0)
			{
				const char * errStr = luaL_checkstring(_logicState, -1);
				LOG_ERROR("%s, code:%d\n", errStr, ret);
			}
		}
		lua_settop(_logicState, top);
		return true;
	}

	bool Logic::worker(shared::InstructPtr & pIns, uint64 now)
	{
		//printf("Logic::worker:%lld, %d\n", now, pIns->type);
		switch (pIns->type)
		{
		case shared::INS_TIMER:
			onTimer(now);
			break;
		case shared::INS_MONGO_JSON:
		case shared::INS_SERVER_JSON:
			{
				lua_getglobal(_logicState, HANDLE_SERVER_JSON);
				if (lua_type(_logicState, -1) == LUA_TFUNCTION)
				{
					lua_pushinteger(_logicState, pIns->src);
					//lua_pushlightuserdata(_logicState, pIns->msg);
					lua_pushstring(_logicState, pIns->handle);
					lua_pushstring(_logicState, (const char*)pIns->msg);
					//lua_pushlstring(_logicState, (const char*)pIns->msg, pIns->size);
					lua_pushinteger(_logicState, pIns->session);
					int ret = lua_pcall(_logicState, 4, 1, 0);
					if (ret != 0)
						LOG_ERROR("%s:%d, %d, %d, %s", luaL_checkstring(_logicState, -1), ret, pIns->src, pIns->type, pIns->handle);
				}
				else
					LOG_ERROR("Invalid ins :%d, %d, %s", pIns->src, pIns->type, pIns->handle);
			}
			break;
		case shared::INS_SERVER_SERI:
			{
				lua_getglobal(_logicState, HANDLE_SERVER_SERI);
				if (lua_type(_logicState, -1) == LUA_TFUNCTION)
				{
					lua_pushinteger(_logicState, pIns->src);
					//lua_pushlightuserdata(_logicState, pIns->msg);
					lua_pushstring(_logicState, pIns->handle);
					lua_pushlightuserdata(_logicState, pIns->msg);
					//lua_pushlstring(_logicState, (const char*)pIns->msg, pIns->size);
					lua_pushinteger(_logicState, pIns->session);
					int ret = lua_pcall(_logicState, 4, 1, 0);
					if (ret != 0)
						LOG_ERROR("%s:%d, %d, %d, %s", luaL_checkstring(_logicState, -1), ret, pIns->src, pIns->type, pIns->handle);
				}
				else
					LOG_ERROR("Invalid ins :%d, %d, %s", pIns->src, pIns->type, pIns->handle);
			}
			break;
		case shared::INS_CLIENT_JSON:
			{
				lua_getglobal(_logicState, HANDLE_CLIENT_JSON);
				if (lua_type(_logicState, -1) == LUA_TFUNCTION)
				{
					lua_pushnumber(_logicState, pIns->roleId);
					lua_pushinteger(_logicState, pIns->msgId);
					//lua_pushlightuserdata(_logicState, pIns->msg);
					//lua_pushstring(_logicState, (char *)pIns->msg);
					lua_pushlstring(_logicState, (char *)pIns->msg, pIns->size);
					lua_pushinteger(_logicState, pIns->size);
					int ret = lua_pcall(_logicState, 4, 1, 0);
					if (ret != 0)
						LOG_ERROR("%s:%d, %d, %lld, %d", luaL_checkstring(_logicState, -1), ret, pIns->src, pIns->roleId, pIns->msgId);
				}
				else
					LOG_ERROR("Invalid ins :%d, %lld, %d", pIns->src, pIns->roleId, pIns->msgId);
			}
			break;
		case shared::INS_CLIENT_PB:
			{
				lua_getglobal(_logicState, HANDLE_CLIENT_PB);
				if (lua_type(_logicState, -1) == LUA_TFUNCTION)
				{
					lua_pushnumber(_logicState, pIns->roleId);
					lua_pushinteger(_logicState, pIns->msgId);
					//lua_pushlightuserdata(_logicState, pIns->msg);
					//lua_pushstring(_logicState, (char *)pIns->msg);
					lua_pushlstring(_logicState, (char *)pIns->msg, pIns->size);
					lua_pushinteger(_logicState, pIns->size);
					int ret = lua_pcall(_logicState, 4, 1, 0);
					if (ret != 0)
						LOG_ERROR("%s:%d, %d, %lld, %d", luaL_checkstring(_logicState, -1), ret, pIns->src, pIns->roleId, pIns->msgId);
				}
				else
					LOG_ERROR("Invalid ins :%d, %lld, %d", pIns->src, pIns->roleId, pIns->msgId);
			}
			break;
		default:
			LOG_ERROR("Invalid ins :%d, %d, %s", pIns->src, pIns->type, pIns->msgId);
		}
		return true;
	}

	void Logic::startTimer(uint64 timeMs)
	{
 		ASSERT(_pServer); 
		shared::Timer & timer = _pServer->getTimer();
		timer.regiestTimer(timeMs, Logic::timerHandle, (void *)this);
	}


	void Logic::timerHandle(void * p)
	{
		//printf("Logic::timerHandle:%lld\n", shared::Timer::getTimeMs());
		Logic * pThis = (Logic *)p;
		ASSERT(pThis->getServer());
		shared::Timer & timer = pThis->getServer()->getTimer();
		//pThis->onTimer(timer.nowMs());
		shared::InstructPtr pIns(new shared::Instruct());
		pIns->src = pThis->getServiceId();
		pIns->des = pThis->getServiceId();
		pIns->type = shared::INS_TIMER;
		pThis->pushInstruct(pIns);
		pThis->startTimer(timer.nowMs() + pThis->_frameTime);
	}

	// lua function
	int Logic::startTimerLua(lua_State *L)
	{
		Logic *pLogic = getService<Logic>(L, 1);
		int top = lua_gettop(L);
		if (top > 1)
		{
			int f = (int)luaL_checknumber(L, 2);
			pLogic->setFrameTime(f);
		}
		
		pLogic->startTimer(0);
		return 0;
	}

	int Logic::doFileLua(lua_State *L)
	{
		Logic *pLogic = getService<Logic>(L, 1);
		ASSERT(pLogic->_logicState);
		int top = lua_gettop(L);
		ASSERT(top > 1);
		pLogic->_mainLogicFile = luaL_checkstring(L, 2);
		int ret = luaL_dofile(pLogic->_logicState, pLogic->_mainLogicFile.c_str());
		if (ret != 0)
		{
			const char *pErrStr = luaL_checkstring(pLogic->_logicState, -1);
			printf("logic doFile error:%s\n", pErrStr);
		}
		lua_pushinteger(L, ret);
		return 1;
	}

	// login:sendIns(name, type, handle, string)
	int Logic::sendInsLua(lua_State *L)
	{
		Logic *pLogic = getService<Logic>(L, 1);
		ASSERT(pLogic->_logicState);
		int top = lua_gettop(L);
		ASSERT(top > 4);
		const char * name = luaL_checkstring(L, 2);
		int type = (int)luaL_checkint(L, 3);
		const char * handle = NULL;
		if (lua_type(L, 4) == LUA_TSTRING)
			handle = luaL_checkstring(L, 4);
		const char * str = luaL_checkstring(L, 5);
		pLogic->sendIns(name, (void*)str, strlen(str), type, handle, 0);
		return 0;
	}

	void Logic::registToLua(lua_State *L)
	{
		shared::ServiceLuaRegister< Logic > lc(L);
		lc.def< &Logic::startTimerLua >("startTimer");
		lc.def< &Logic::doFileLua >("doFile");
		lc.def< &Logic::sendInsLua >("sendIns");

	}

	//"name", INS_TYPE, handle, strBuf, length, roleId
	int Logic::sendInsLuaMy(lua_State *L)
	{
		int top = lua_gettop(L);
		ASSERT(top > 4);
		Logic *pLogic = (Logic*)lua_touserdata(L, 1);
		ASSERT(pLogic && pLogic->_logicState);
		const char * name = luaL_checkstring(L, 2);
		int type = (int)luaL_checkint(L, 3);
		const char * handle = lua_tolstring(L, 4, (size_t*)NULL);
		const char * str = NULL;
		if (type == shared::INS_SERVER_SERI)
			str = (char *)lua_touserdata(L, 5);
		else
			str = lua_tolstring(L, 5, (size_t*)NULL);
		int64 roleId = 0;
		uint32 msgId = 0;
		switch (type)
		{
		case shared::INS_SERVER_SERI:
		case shared::INS_SERVER_JSON:
			
			break;
		case shared::INS_CLIENT_JSON:
		case shared::INS_CLIENT_PB:
			msgId = luaL_checkint(L, 4);
			//str = (char *)lua_touserdata(L, 5);
			roleId = luaL_checknumber(L, 7);
			break;
		}
		
		int len = 0;
		if (top >= 6 && lua_type(L, 6) == LUA_TNUMBER)
			len = luaL_checkint(L, 6);
		else if (str)
			len = strlen(str) + 1;
		
		//pLogic->sendIns(name, (void*)str, len, type, handle, 0);
		shared::InstructPtr pIns(new shared::Instruct());
		pIns->src = pLogic->getServiceId();
		pIns->roleId = roleId;
		pIns->msgId = msgId;
		pIns->msg = (void*)str;
		pIns->size = len;
		pIns->type = type;
		pIns->session = pLogic->getIncId();
		if (handle != NULL)
			strncpy(pIns->handle, handle, HANDLE_NAME_SIZE);
		pLogic->sendIns(name, pIns);

		lua_pushinteger(pLogic->_logicState, pIns->session);
		return 1;
	}

	int Logic::getTimeMsMy(lua_State *L)
	{
		Logic *pLogic = (Logic*)lua_touserdata(L, 1);
		ASSERT(pLogic->_logicState);
		uint64 nowMs = shared::Timer::getTimeMs();
		lua_pushnumber(pLogic->_logicState, (lua_Number)nowMs);
		return 1;
	}

}