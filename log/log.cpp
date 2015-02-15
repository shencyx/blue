#include "log.h"
#include "shared/GlobalDefine.h"
#include "shared/Error.h"

namespace logS
{
	const char Log::serviceName[] = "log_service";

	Log::Log(lua_State * L, const char * pName):
		Service(L, pName),
		_log(false),
		_isPrint(true)
	{
		_log.Init("log_service.log");
	}

	bool Log::onTimer(uint64 now)
	{
		printf("log onTimer");
		Service::onTimer(now);
		return true;
	}

	bool Log::worker(shared::InstructPtr & pIns, uint64 now)
	{
		//printf("Log::worker:%lld\n", now);
		if (pIns->type == shared::INS_LOG)
		{
			//printf("---->>>Log::INS_LOG:%s\n", (char*)pIns->msg);
			//LOG_PRINT((char*)pIns->msg);
			if (_isPrint)
				_log.write((char*)pIns->msg);
			else
				_log.writeSilent((char*)pIns->msg);
		}
		return true;
	}

	void Log::registToLua(lua_State *L)
	{
		shared::ServiceLuaRegister< Log > lc(L);
		lc.def< &Log::setPrintLua >("setPrint");
	}

	int Log::setPrintLua(lua_State *L){ 
		Log *pLogic = getService<Log>(L, 1);
		_isPrint = 0 != lua_toboolean(L, 2);

		return 0;
	}
}