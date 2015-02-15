/********************************************************************
	filename: 	WorkThread.cpp
	created:	2013/08/15
	author:		shencyx@126.com
	
	purpose:	
*********************************************************************/
#pragma once
#include "Common.h"
#include "Server.h"
#include "Service.h"

namespace shared {

	
	class SERVER_DECL WorkThread : public ThreadBase
	{
	public:
		WorkThread(Service * pService, uint64 now):_pService(pService),_now(now){}


		bool run()
		{
			int priority = 0;
			while (true)
			{
				InstructPtr pIns = _pService->popInstruct();
				if (pIns.get())
				{
					_pService->worker(pIns, _now);
					pIns->freeMsg();
				}
				else
				{
					_pService->setInWorkQueue(false);
					break;
				}
				if (++priority > _pService->getPriority())
				{
					_pService->getServer()->pushWorkQueue(_pService);
					break;
				}
			}
			return true;
		}

		Service * _pService;
		uint64 _now;
	};

}