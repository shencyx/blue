/********************************************************************
	filename: 	my_socket.h
	created:	2013/08/13
	author:		shencyx@126.com
	
	purpose:	
*********************************************************************/
#pragma once
#include "shared/Common.h"
#include "shared/network/Network.h"
#include "shared/Service.h"



namespace gate {

	const static uint32 DEF_BUF_SIZE = 1024 * 1024;

	class Gate;

	class MySocket : public Socket
	{
	public:
		MySocket(SOCKET fd, void* args, uint32 sendbuffersize = DEF_BUF_SIZE, uint32 recvbuffersize = DEF_BUF_SIZE);
		virtual ~MySocket(){}

		// Called when data is received.
		virtual void OnRead();

		// Called when a connection is first successfully established.
		virtual void OnConnect();

		// Called when the socket is disconnected from the client (either forcibly or by the connection dropping)
		virtual void OnDisconnect();

		void doDisconnect(){}
		void send(uint32 msgId, char * buf, uint32 size);
		void dispatch(shared::InstructPtr & pIns);

		void setState(int32 state){ _state = state; }
		void setRoleId(int64 roleId){ _roleId = roleId; }
		void getIP(char * addrBuf);
	private:
		int32 _state;			//0新连接，1已验证，2验证不通过, -1断开
		int32 _selectorType;	//定向收到的信息怎么分发
		uint32 _packet;			//数据包计算
		int64 _roleId;			//对应roleId
		Gate * _gate;
	};

}