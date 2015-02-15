/********************************************************************
	filename: 	conn_socket.h
	created:	2013/09/10
	author:		shencyx@126.com
	
	purpose:	
*********************************************************************/
#pragma once
#include "shared/Common.h"
#include "shared/network/Network.h"
#include "shared/Service.h"


namespace connector {

	const static uint32 DEF_BUF_SIZE = 1024 * 1024;
	class Connector;

	class ConnSocket : public Socket
	{
	public:
		ConnSocket(SOCKET fd, Connector * connector, uint32 sendbuffersize = DEF_BUF_SIZE, uint32 recvbuffersize = DEF_BUF_SIZE);
		virtual ~ConnSocket(){}

		// Called when data is received.
		virtual void OnRead();

		// Called when a connection is first successfully established.
		virtual void OnConnect();

		// Called when the socket is disconnected from the client (either forcibly or by the connection dropping)
		virtual void OnDisconnect();

		void doDisconnect(){}
		void send(uint32 msgId, char * buff, uint32 size);

		void dispatch(shared::InstructPtr & pIns);

	private:
		int _state;
		int _selectorType;
		uint32 _id;
		Connector * _connector;
	};




}