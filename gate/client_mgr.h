/********************************************************************
	filename: 	socket_mgr.h
	created:	2013/12/20
	author:		shencyx@126.com
	
	purpose:	
*********************************************************************/
#pragma once
#include "shared/Common.h"
#include "my_socket.h"
#include "shared/Singleton2.h"

namespace gate{

	class ClientMgr : public shared::Singleton<ClientMgr>
	{
	public:
		typedef HM_NAMESPACE::hash_map<SOCKET, MySocket*> SocketMap;
		typedef HM_NAMESPACE::hash_map<int64, SOCKET> ClientMap;

	public:
		ClientMgr();

		void insert(SOCKET socketId, MySocket * socket);
		void erase(SOCKET socketId);
		MySocket * getSocketByFd(SOCKET socketId);
		MySocket * getSocket(int64 roleId);

		void makePair(int64 roleId, SOCKET socketId);


	private:
		SocketMap _socketMap;
		ClientMap _clientMap;
	};

}//gate