/********************************************************************
	filename: 	gate.h
	created:	2013/08/13
	author:		shencyx@126.com
	
	purpose:	对外网络服务
*********************************************************************/

#pragma once
#include <lua.hpp>
#include "shared/Common.h"
#include "shared/Service.h"

struct Thread;

namespace gate
{
	class Gate : public shared::Service
	{
	public:
		Gate(lua_State * L, const char * pName);
		virtual ~Gate(){ printf("~Gate\n"); }
		virtual bool onTimer(uint64 now);
		virtual bool worker(shared::InstructPtr & pIns, uint64 now);

		void sendPacket(shared::InstructPtr & pIns);

		void start();
		void stop();

		// lua function
		int setListenLua(lua_State *L);
		int startLua(lua_State *L);
		int stopLua(lua_State *L);

		static void registToLua(lua_State *L);
		static const char serviceName[];

	private:
		string _ip;
		int _port;	//
		Thread * _listenThread;
	};
}