#include "Gate.h"
#include "shared/GlobalDefine.h"
#include "my_socket.h"
#include "shared/Singleton.h"
#include "shared/CmdMgr.h"
#include "cmd_handles.h"
#include "client_mgr.h"

namespace gate
{

	const char Gate::serviceName[] = "gate_service";

	Gate::Gate(lua_State * L, const char * pName):
		Service(L, pName),
		_port(0),
		_listenThread(0)
	{
		shared::CmdMgr::getSingleton();
		registerHandles();
	}

	bool Gate::onTimer(uint64 now)
	{
		printf("Gate onTimer");
	
		return true;
	}

	bool Gate::worker(shared::InstructPtr & pIns, uint64 now)
	{
		//printf("Gate::worker type:%d :%lld\n", pIns->type, now);
		switch (pIns->type)
		{
		case shared::INS_TIMER:
			break;
		case shared::INS_SERVER_CMD:
			shared::CmdMgr::getSingleton().handleCmd(this, pIns);
			break;
		case shared::INS_SERVER_JSON:
			shared::CmdMgr::getSingleton().handleCmdStr(this, pIns);
			break;
		case shared::INS_SERVER_SERI:
			break;
		case shared::INS_CLIENT_JSON:
		case shared::INS_CLIENT_PB:
			{
				//_connSocket.send(pIns->msgId, pIns->msg, pIns->size);
				sendPacket(pIns);
			}
			break;
		default:
			LOG_ERROR("Invalid ins :%d, %d, %s", pIns->src, pIns->type, pIns->msgId);
		}
		return true;
	}

	void Gate::sendPacket(shared::InstructPtr & pIns)
	{
		MySocket * pSocket = ClientMgr::getSingleton().getSocket(pIns->roleId);
		if (pSocket != NULL)
		{
			pSocket->send(pIns->msgId, (char*)pIns->msg, pIns->size);
		}
	}

	void Gate::start()
	{
		if (_ip.empty())
		{
			LOG_ERROR("no ip, no part, setListen first");
		} 
		else
		{
			LOG_PRINT("GATE listen: %s:%d", _ip.c_str(), _port);
			_listenThread = ThreadPool.StartThread(new ListenSocket<MySocket>(_ip.c_str(), _port, (void*)this));
			sSocketMgr.SpawnWorkerThreads();
		}
		
	}

	void Gate::stop()
	{
		sSocketMgr.CloseAll();
		sSocketMgr.ShutdownThreads();
		if (_listenThread)
		{
			ThreadPool.ThreadExit(_listenThread);
			_listenThread = NULL;
		}
	}

	//////////////////////// lua

	void Gate::registToLua(lua_State *L)
	{
		shared::ServiceLuaRegister< Gate > lc(L);
		lc.def< &Gate::setListenLua >("setListen");
		lc.def< &Gate::startLua >("start");
		lc.def< &Gate::stopLua >("stop");
	}

	int Gate::setListenLua(lua_State *L)
	{
		Gate *pGate = getService<Gate>(L, 1);
		pGate->_ip = luaL_checkstring(L, 2);
		pGate->_port = luaL_checkint(L, 3);

		return 0;
	}

	int Gate::startLua(lua_State *L)
	{ 
		Gate *pGate = getService<Gate>(L, 1);
		pGate->start();

		return 0;
	}

	int Gate::stopLua(lua_State *L)
	{ 
		Gate *pGate = getService<Gate>(L, 1);
		pGate->stop();

		return 0;
	}
}