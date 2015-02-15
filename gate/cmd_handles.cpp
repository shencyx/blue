#include "shared/Common.h"
#include "shared/GlobalDefine.h"
#include "shared/Service.h"
#include "shared/CmdMgr.h"
#include "common/cpp/gate/cmd_struct.h"
#include "bson.h"
#include "client_mgr.h"
#include "my_socket.h"

//using namespace bson;


namespace gate{

	//int32 handleLogin(shared::InstructPtr & pIns)
	//{
	//	ASSERT(pIns->size == sizeof(protocol::L2GLogin));
	//	protocol::L2GLogin *pL2GLogin = (protocol::L2GLogin*)pIns->msg;

	//	return 0;
	//}

	static int32 handleLogin(shared::Service * pService, shared::InstructPtr & pIns)
	{
		LOG_PRINT("handle login msg:%s", (const char*)pIns->msg);
		bson::bo msg = bson::fromjson((const char*)pIns->msg);

		int64 roleId = msg["roleId"].numberLong();
		int32 fd = msg["fd"].Int();
		int32 code = msg["code"].Int();
		int32 state = msg["state"].Int();

		MySocket *pSocket = ClientMgr::getSingleton().getSocketByFd((SOCKET)fd);
		if (pSocket == NULL)
		{

			return 0;
		}

		if (code != 0)
		{

			return 0;
		} 
		pSocket->setState(1);
		pSocket->setRoleId(roleId);
		ClientMgr::getSingleton().makePair(roleId, pSocket->GetFd());
		char ipaddr[32] = {0};
		pSocket->getIP(ipaddr);
		bson::bob backMsg;
		backMsg.append("code", 0);
		backMsg.append("roleId", roleId);
		backMsg.append("ip", ipaddr);
		string buff = backMsg.obj().jsonString();
		pService->sendIns(pIns->src, (void*)buff.data(), buff.length() + 1, shared::INS_SERVER_JSON, "G2L_ACCEPT", 0);
		return 0;
	}

	void registerHandles()
	{
		//ASSERT(shared::CmdMgr::getSingleton().registerCmdHandle(protocol::L2G_LOGIN, handleLogin));
		ASSERT(shared::CmdMgr::getSingleton().registerCmdHandle("L2G_LOGIN", handleLogin));
	}

}//gate