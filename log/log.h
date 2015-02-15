/********************************************************************
	filename: 	sLog.h
	created:	2013/07/13
	author:		shencyx@126.com
	
	purpose:	日志服务
*********************************************************************/

#pragma once
#include <lua.hpp>
#include "shared/Common.h"
#include "shared/Service.h"
#include "shared/log/Log.h"

namespace logS
{
	class Log : public shared::Service
	{
	public:
		Log(lua_State * L, const char * pName);
		virtual ~Log(){ printf("~log\n"); }
		virtual bool onTimer(uint64 now);
		virtual bool worker(shared::InstructPtr & pIns, uint64 now);



		// lua function
		int setPrintLua(lua_State *L);

		static void registToLua(lua_State *L);
		static const char serviceName[];

	private:
		oLog _log;
		bool _isPrint;
	};
}