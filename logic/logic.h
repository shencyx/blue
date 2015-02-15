/********************************************************************
	filename: 	logic.h
	created:	2013/07/19
	author:		shencyx@126.com
	
	purpose:	单个逻辑运行单元
*********************************************************************/
#pragma once
#include <lua.hpp>
#include "shared/Common.h"
#include "shared/Service.h"

namespace logic
{
	class Logic : public shared::Service
	{
	public:
		Logic(lua_State * L, const char * pName);
		virtual ~Logic();

		void initMy();
		virtual bool onTimer(uint64 now);
		virtual bool worker(shared::InstructPtr & pIns, uint64 now);

		void setFrameTime(uint32 t) { _frameTime = t; }
		void startTimer(uint64 timeMs);

		
		// lua function
		int startTimerLua(lua_State *L);
		int doFileLua(lua_State *L);
		int sendInsLua(lua_State *L);

		static void timerHandle(void * p);
		static void registToLua(lua_State *L);
		static const char serviceName[];

		//
		static int sendInsLuaMy(lua_State *L);
		static int getTimeMsMy(lua_State *L);

	private:
		lua_State *_logicState;
		uint32 _frameTime;
		string _mainLogicFile;
	};
}