#include <signal.h>
#include "Server.h"
#include "log/Log.h"
#include "WorkThread.h"

using namespace shared;

namespace shared
{
	static void signalFun(int sig)
	{
		LOG_PRINT("signalFun(%d)", sig);
		::signal(2, signalFun);
		::signal(15, signalFun);
		Server::_self->signal(sig);
	}


	const char Server::serviceName[] = "blue_server";
	Server * Server::_self = NULL;

	Server::Server(lua_State * L, const char * pName):
		Service(L, Server::serviceName),
		_frameTime(10),
		_serviceIdDis(0),
		_start(false)
	{
		Server::_self = this;
	}

	Server::~Server()
	{
		stop();
	}

	bool Server::start()
	{
		LOG_PRINT("start server!");
		//::signal(2, SIG_IGN);
		::signal(2, signalFun);
		::signal(15, signalFun);
		_start = true;
		while (_start)
		{
			//uint64 t1 = Timer::getTimeMs();
			_timer.refreshNow();
			uint64 t1 = _timer.nowMs();
			_timer.loop();
			loop(t1);
			_timer.refreshNow();
			uint64 t2 = _timer.nowMs();
			//uint64 t2 = Timer::getTimeMs();
			int sT = (int)(_frameTime - (t2 - t1));
			if (sT > 0)
				Timer::sleep(sT);
		}

		return true;
	}

	void Server::stop()
	{
		LOG_PRINT("stop server!");
	}

	/*
	bool Server::loop(uint64 now)
	{
		//printf("Server::loop:%llu\n", now);
		//ServiceMap::iterator it = _serviceMap.begin();
		//for (; it != _serviceMap.end(); ++it)
		//{
		//	it->second->onTimer(now);
		//}
		//
		int priority = 0;
		while (!_workQueue.empty())
		{
			priority = 0;
			Service * pService = _workQueue.front();
			if (pService == NULL)
			{
				return true;
			}
			_workQueue.pop();
			while (pService)
			{
				//printf("pService:%s\n", pService->getServiceName().c_str());
				InstructPtr pIns = pService->popInstruct();
				if (pIns.get())
				{
					pService->worker(pIns, now);
				}
				else
				{
					pService->setInWorkQueue(false);
					break;
				}
				if (++priority > pService->getPriority())
				{
					_workQueue.push(pService);
					break;
				}
			}
		}
		return true;
	}
	*/

	bool Server::loop(uint64 now)
	{

		while (!_workQueue.empty())
		{
			_mutex.lock();
			Service * pService = _workQueue.front();
			if (pService == NULL)
			{
				return true;
			}
			_workQueue.pop();
			_mutex.unlock();

			ThreadPool.ExecuteTask(new shared::WorkThread(pService, now));
		}
		return true;
	}

	void Server::signal(int sig)
	{
		if (sig == 2 || sig == 15)
		{
			_start = false;
		}
	}

	bool Server::registService(Service * pService, const string & name)
	{
		//还需要判断重复注册等
		_serviceMap.insert(std::make_pair(name, pService));
		pService->setServer(this);
		int32 sId = distributionServiceId();
		pService->setId(sId);
		_serviceIdMap.insert(std::make_pair(sId, pService));


		return true;
	}

	void Server::sendIns(const string & name, InstructPtr pIns)
	{
		ServiceMap::iterator it = _serviceMap.find(name);
		if (it != _serviceMap.end())
		{
			it->second->pushInstruct(pIns);
		}
		else
			LOG_ERROR("invalid service name:%s", name.c_str()); //error
	}

	void Server::sendIns(int idDes, InstructPtr pIns)
	{
		ServiceIdMap::iterator it = _serviceIdMap.find(idDes);
		if (it != _serviceIdMap.end())
		{
			it->second->pushInstruct(pIns);
		}
		else
			LOG_ERROR("invalid service id:%d", idDes); //error
	}

	void Server::pushWorkQueue(Service * pService)
	{
		_mutex.lock();
		_workQueue.push(pService);
		_mutex.unlock();
	}


	int Server::startLua(lua_State *L)
	{
		Server *pServer = getService<Server>(L, 1);
		//int f = (int)luaL_checknumber(L, 2);

		pServer->start();
		return 0;
	}

	int Server::registServiceLua(lua_State *L)
	{
		Server *pServer = getService<Server>(L, 1);
		luaL_checktype(L, 2, LUA_TTABLE);
		lua_getfield(L, 2, "__cobj");
		shared::Service ** pService = (shared::Service**)lua_touserdata(L, -1);
		ASSERT(*pService != NULL);
		//const char * name = luaL_checkstring(L, 3);
		pServer->registService(*pService, (*pService)->getName());
		return 0;
	}

	int Server::setFrameTimeLua(lua_State *L)
	{
		Server *pServer = getService<Server>(L, 1);
		int f = (int)luaL_checknumber(L, 2);

		pServer->setFrameTime(f);
		return 0;
	}

	void Server::registToLua(lua_State *L)
	{
		shared::ServiceLuaRegister< Server > lc(L);
		lc.def< &Server::registServiceLua >("registService");
		lc.def< &Server::startLua >("start");
		lc.def< &Server::setFrameTimeLua >("setFrameTime");
	}

}