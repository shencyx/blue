/********************************************************************
	filename: 	CmdMgr.h
	created:	2013/12/19
	author:		shencyx@126.com
	
	purpose:	
*********************************************************************/
#pragma once
#include "Common.h"
#include "Server.h"
#include "Service.h"
#include "Singleton2.h"


namespace shared {

	class CmdMgr: public Singleton<CmdMgr>
	{
	public:
		typedef int32(*CMD_HANDLE)(shared::Service * pService, InstructPtr & pIns);
		typedef HM_NAMESPACE::hash_map<uint32, CMD_HANDLE> HandleMap;
		typedef HM_NAMESPACE::hash_map<string, CMD_HANDLE> HandleMapStr;

		bool registerCmdHandle(uint32 cmd, CMD_HANDLE handle)
		{
			if (_cmdMap.find(cmd) != _cmdMap.end())
			{
				return false;
			}
			_cmdMap.insert(make_pair(cmd, handle));
			return true;
		}

		int32 handleCmd(shared::Service * pService, InstructPtr & pIns)
		{
			HandleMap::iterator it = _cmdMap.find(pIns->msgId);
			if (it == _cmdMap.end())
			{
				return -1;
			}
			return it->second(pService, pIns);
		}

		bool registerCmdHandle(const string & cmd, CMD_HANDLE handle)
		{
			if (_cmdMapStr.find(cmd) != _cmdMapStr.end())
			{
				return false;
			}
			_cmdMapStr.insert(make_pair(cmd, handle));
			return true;
		}

		int32 handleCmdStr(shared::Service * pService, InstructPtr & pIns)
		{
			HandleMapStr::iterator it = _cmdMapStr.find(pIns->handle);
			if (it == _cmdMapStr.end())
			{
				LOG_ERROR("can't find hadle:%s", pIns->handle);
				return -1;
			}
			return it->second(pService, pIns);
		}

	private:
		HandleMap _cmdMap;
		HandleMapStr _cmdMapStr;
	};


}//shared
