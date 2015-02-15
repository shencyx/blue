/********************************************************************
	filename: 	GlobalPacket.h
	created:	2013/07/27
	author:		shencyx@126.com
	
	purpose:	���ýṹ�ݶ���
*********************************************************************/
#pragma once
#include "Common.h"
#include "util/SharedPtr.h"

namespace shared
{
	enum INS_TYPE
	{
		INS_UNKNOW = 0,
		INS_TIMER = 1,	//��ʱ����Ϣ
		INS_LOG = 2,	//��־����
		INS_MONGO = 3,	//mongo���ݿ�
		INS_MONGO_JSON = 4,	//mongo���ݿ�JSON ��ʽ
		INS_MONGO_BSON = 5,	//mongo���ݿ�BSON ��ʽ
		INS_SERVER_JSON = 6,	//������֮��ͨѶ
		INS_SERVER_SERI = 7,	//������֮��ͨѶ
		INS_CLIENT_JSON = 8,	//�ͻ���ͨѶ
		INS_CLIENT_PB = 9,		//�ͻ���ͨѶ
		INS_SERVER_CMD = 10,	//�Զ�������
		INS_CLIENT_CMD = 11,
		INS_SERVER_BSON = 12,



		// 1000���ڱ���
		INS_USER = 1001,
	};

	// mongoָ�� json��ʽ
	struct SERVER_DECL MongoJsonCmd
	{
		MongoJsonCmd(){ memset(this, 0, sizeof(*this)); }
		int32 src;
		int32 des;
		int32 type;
		uint32 session;
		void * msg;
		uint32 size;
	};
	typedef sharedPtr<MongoJsonCmd> MongoJsonCmdPtr;

}



