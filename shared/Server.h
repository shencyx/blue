/********************************************************************
	filename: 	server.h
	created:	2013/07/12
	author:		shencyx@126.com
	
	purpose:	用于加载各驱动其它服务
*********************************************************************/
#pragma once

#include <map>
#include <queue>
#include "Common.h"
#include "Service.h"
#include "util/Timer.h"

using std::make_pair;


namespace shared
{
	class SERVER_DECL Server : public Service
	{
	public:
		typedef std::map<string, Service *> ServiceMap;
		typedef std::map<int32, Service *> ServiceIdMap;
		typedef std::queue<Service *> WorkQueue;

	public:
		Server(lua_State * L, const char * pName = NULL);
		virtual ~Server();

		bool start();
		void stop();
		inline bool loop(uint64 now);

		virtual void signal(int sig);

		bool registService(Service * pService, const string & name);
		void sendIns(const string & name, InstructPtr pIns);
		void sendIns(int idDes, InstructPtr pIns);
		void pushWorkQueue(Service * pService);

		//set
		void setFrameTime(uint32 ft){ _frameTime = ft; }

		//get
		int32 distributionServiceId(){ return ++_serviceIdDis; }
		Timer & getTimer(){ return _timer; }

		// lua function
		int startLua(lua_State *L);
		int registServiceLua(lua_State *L);
		int setFrameTimeLua(lua_State *L);

		static void registToLua(lua_State *L);
		static const char serviceName[];

		static Server * _self;	//只为了信号通知
	private:
		uint32 _frameTime;
		int32 _serviceIdDis;
		bool _start;
		ServiceMap _serviceMap;
		ServiceIdMap _serviceIdMap;
		WorkQueue _workQueue;
		Timer _timer;
		Mutex _mutex;
	};
}