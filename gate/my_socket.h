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
		int32 _state;			//0�����ӣ�1����֤��2��֤��ͨ��, -1�Ͽ�
		int32 _selectorType;	//�����յ�����Ϣ��ô�ַ�
		uint32 _packet;			//���ݰ�����
		int64 _roleId;			//��ӦroleId
		Gate * _gate;
	};

}