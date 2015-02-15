#include "conn_socket.h"
#include "shared/GlobalDefine.h"
#include "selector.h"
#include "connector.h"

namespace connector {
	const static uint32 PACK_HEAD_SIZE = 6;
	const static uint32 PACK_MAX_SIZE = 0xffff;
	const static uint32 MSG_MAX_SIZE = PACK_MAX_SIZE - PACK_HEAD_SIZE;


ConnSocket::ConnSocket(SOCKET fd, Connector * connector, uint32 sendbuffersize, uint32 recvbuffersize):
	Socket(fd, sendbuffersize, recvbuffersize),
	_state(0),
	_selectorType(1),
	_id(0),
	_connector(connector)
{

}

void ConnSocket::OnRead()
{
	//printf("OnRead size:%d\n", readBuffer.GetSize());
	size_t size = readBuffer.GetSize();
	if (size < PACK_HEAD_SIZE)
		return;
	char * rBuff = (char*)readBuffer.GetBufferStart();
	uint16 packSize = 0;
	memcpy(&packSize, rBuff, 2);
	if (packSize > PACK_MAX_SIZE)
		doDisconnect();
	if (size < packSize)
		return;
	uint32 msgId = 0;
	readBuffer.Read(&packSize, 2);
	readBuffer.Read(&msgId, 4);
	uint16 msgSize = packSize - PACK_HEAD_SIZE;
	char * msgBuf = new char[msgSize];
	if (!readBuffer.Read(msgBuf, msgSize))
		doDisconnect();

	//printf("connSocket OnRead:size:%d, %d, %s, %d\n", size, msgSize, msgBuf, readBuffer.GetSize());
	//DumpBuffer(msgBuf, msgSize);
	shared::InstructPtr pIns(new shared::Instruct());
	pIns->src = _connector->getServiceId();
	pIns->roleId = _id;
	pIns->msgId = msgId;
	pIns->msg = msgBuf;
	pIns->size = msgSize;
	pIns->type = shared::INS_CLIENT_PB;
	pIns->session = 0;
	dispatch(pIns);
}

void ConnSocket::OnConnect()
{
	printf("ConnSocket::onConnect--\n");
}

void ConnSocket::OnDisconnect()
{
	printf("ConnSocket::OnDisconnect\n");

}

void ConnSocket::send(uint32 msgId, char * buff, uint32 size)
{
	//printf("send: %d, %s, %d\n", msgId, buff, size);
	if (size >= MSG_MAX_SIZE)
	{
		LOG_ERROR("Invalid size :%d, %d", msgId, size);
		return;
	}
	
	uint16 packSize = size + PACK_HEAD_SIZE;
	if (!Send((uint8*)&packSize, 2))
		LOG_ERROR("send error :%d, %d", msgId, size);
	if (!Send((uint8*)&msgId, 4))
		LOG_ERROR("send error :%d, %d", msgId, size);
	if (!Send((uint8*)buff, size))
		LOG_ERROR("send error :%d, %d", msgId, size);
}

void ConnSocket::dispatch(shared::InstructPtr & pIns)
{
	switch (_selectorType)
	{
	case 1:
		{
			string desName = selector1(pIns->msgId);
			_connector->sendIns(desName, pIns);
		}
		break;
	}
}

}// connector