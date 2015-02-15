/********************************************************************
	filename: 	connector.h
	created:	2013/09/10
	author:		shencyx@126.com
	
	purpose:	socket¡¨Ω”
*********************************************************************/

#pragma once
#include <lua.hpp>
#include "shared/Common.h"
#include "shared/Service.h"
#include "conn_socket.h"


namespace connector
{
	class Connector : public shared::Service
	{
	public:
		Connector(lua_State * L, const char * pName);
		virtual ~Connector(){ printf("~Connector\n"); }
		virtual bool onTimer(uint64 now);
		virtual bool worker(shared::InstructPtr & pIns, uint64 now);

		bool connect(const string & ip, int port);
		bool reconnect();

		// lua function
		int connectLua(lua_State *L);

		static void registToLua(lua_State *L);
		static const char serviceName[];

	private:
		string _ip;
		int _port;	//
		ConnSocket _connSocket;
	};
}