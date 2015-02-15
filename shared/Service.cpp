#include "Service.h"
#include "Server.h"


namespace shared
{
	Service::Service(lua_State * L, const char * pName):
		_pServer(NULL),
		_inWorkQueue(false),
		_parentState(L),
		_id(0),
		_priority(10),
		_incId(1),
		_name(pName)
	{
		
	}

	void Service::pushInstruct(InstructPtr & ins)
	{
		ins->mallocMsg();
		_insQueue.push(ins);
		if (!_inWorkQueue.GetVal())
		{
			_pServer->pushWorkQueue(this);
			setInWorkQueue(true);
		}
	}

	void Service::sendIns(const string & name, void * msg, uint32 size, int32 insType, const char * handle, uint32 session)
	{
		InstructPtr pIns(new Instruct());
		pIns->src = _id;
		pIns->msg = msg;
		pIns->size = size;
		pIns->type = insType;
		pIns->session = session;
		if (handle)
			strncpy(pIns->handle, handle, HANDLE_NAME_SIZE);
		_pServer->sendIns(name, pIns);
	}

	void Service::sendIns(int idDes, void * msg, uint32 size, int32 insType, const char * handle, uint32 session)
	{
		InstructPtr pIns(new Instruct());
		pIns->src = _id;
		pIns->msg = msg;
		pIns->size = size;
		pIns->type = insType;
		pIns->session = session;
		if (handle)
			strncpy(pIns->handle, handle, HANDLE_NAME_SIZE);
		_pServer->sendIns(idDes, pIns);
	}

	void Service::sendIns(const string & name, InstructPtr pIns)
	{
		_pServer->sendIns(name, pIns);
	}

	void Service::sendIns(int idDes, InstructPtr pIns)
	{
		_pServer->sendIns(idDes, pIns);
	}
}