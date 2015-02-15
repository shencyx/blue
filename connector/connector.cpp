#include "connector.h"
#include "shared/GlobalDefine.h"

namespace connector
{

	const char Connector::serviceName[] = "connector_service";

	Connector::Connector(lua_State * L, const char * pName):
		Service(L, pName),
		_port(0),
		_connSocket(0, this)
	{
		if (!sSocketMgr.IsRun())
			sSocketMgr.SpawnWorkerThreads(1);
	}

	bool Connector::onTimer(uint64 now)
	{
		printf("Connector onTimer");

		return true;
	}

	bool Connector::worker(shared::InstructPtr & pIns, uint64 now)
	{
		printf("Connector::worker:%lld\n", now);
		switch (pIns->type)
		{
		case shared::INS_TIMER:
			break;
		case shared::INS_SERVER_JSON:
		case shared::INS_SERVER_SERI:
		case shared::INS_CLIENT_JSON:
		case shared::INS_CLIENT_PB:
			{
				_connSocket.send(pIns->msgId, (char*)pIns->msg, pIns->size);
			}
			break;
		default:
			LOG_ERROR("Invalid ins :%d, %d, %s", pIns->src, pIns->type, pIns->msgId);
		}
		return true;
	}

	bool Connector::connect(const string & ip, int port)
	{
		_ip = ip;
		_port = port;
		return _connSocket.Connect(_ip.c_str(), _port);
	}

	bool Connector::reconnect()
	{
		return _connSocket.Connect(_ip.c_str(), _port);
	}

	//////////////////////// lua

	void Connector::registToLua(lua_State *L)
	{
		shared::ServiceLuaRegister< Connector > lc(L);
		lc.def< &Connector::connectLua >("connect");

	}

	int Connector::connectLua(lua_State *L)
	{
		Connector *pConnector = getService<Connector>(L, 1);
		pConnector->_ip = luaL_checkstring(L, 2);
		pConnector->_port = luaL_checkint(L, 3);
		bool b = pConnector->reconnect();
		lua_pushboolean(L, b);
		return 1;
	}

}