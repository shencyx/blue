#include "my_socket.h"
#include "shared/GlobalDefine.h"
#include "gate.h"
#include "shared/GlobalDefine.h"
#include "selector.h"
#include "client_mgr.h"

namespace gate {
	const static uint32 PACK_HEAD_SIZE = 6;
	const static uint32 PACK_MAX_SIZE = 0xffff;
	const static uint32 MSG_MAX_SIZE = PACK_MAX_SIZE - PACK_HEAD_SIZE;

MySocket::MySocket(SOCKET fd, void* args, uint32 sendbuffersize, uint32 recvbuffersize):
	Socket(fd, sendbuffersize, recvbuffersize),
	_state(0),
	_selectorType(1),
	_packet(0),
	_roleId(0),
	_gate((Gate*)args)
{

}

void MySocket::OnRead()
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
	
	//printf("OnRead 2:size:%d, %s, %d\n", size, msgBuf, readBuffer.GetSize());
	shared::InstructPtr pIns(new shared::Instruct());
	pIns->src = _gate->getServiceId();
	if (_state == 1)
		pIns->roleId = _roleId;
	else
		pIns->roleId = GetFd();
	pIns->msgId = msgId;
	pIns->msg = msgBuf;
	pIns->size = msgSize;
	pIns->type = shared::INS_CLIENT_PB;
	pIns->session = 0;
	dispatch(pIns);
}

void MySocket::OnConnect()
{
	printf("MySocket onConnect\n");
	ClientMgr::getSingleton().insert(GetFd(), this);
}

void MySocket::OnDisconnect()
{
	printf("MySocket OnDisconnect\n");
	ClientMgr::getSingleton().erase(GetFd());
}

void MySocket::send(uint32 msgId, char * buf, uint32 size)
{
	//printf("send: %d, %s, %d\n", msgId, buf, size);
	if (size >= MSG_MAX_SIZE)
	{
		LOG_ERROR("Invalid size :%d, %d", msgId, size);
		return;
	}
	//DumpBuffer(buf, size);
	uint16 packSize = size + PACK_HEAD_SIZE;
	if (!Send((uint8*)&packSize, 2))
		LOG_ERROR("send error :%d, %d", msgId, size);
	if (!Send((uint8*)&msgId, 4))
		LOG_ERROR("send error :%d, %d", msgId, size);
	if (!Send((uint8*)buf, size))
		LOG_ERROR("send error :%d, %d", msgId, size);
}

void MySocket::dispatch(shared::InstructPtr & pIns)
{
	switch (_selectorType)
	{
	case 1:
		{
			const string & desName = selector1(pIns->msgId);
			_gate->sendIns(desName, pIns);
		}
		break;
	}
	
}

void MySocket::getIP(char * addrBuf)
{
	uint32 ip = (uint32)GetRemoteAddress().S_un.S_addr;
	int8 i4 = (ip >> 24) & 0xff;
	int8 i3 = (ip >> 16) & 0xff;
	int8 i2 = (ip >> 8) & 0xff;
	int8 i1 = ip & 0xff;
	sprintf(addrBuf, "%d.%d.%d.%d", i4, i3, i2, i1);
}

}// gate