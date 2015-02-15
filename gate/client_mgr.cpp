#include "client_mgr.h"
#include "shared/log/Log.h"

namespace gate{

	ClientMgr::ClientMgr()
	{

	}

	void ClientMgr::insert(SOCKET socketId, MySocket * socket)
	{
		if (_socketMap.find(socketId) != _socketMap.end())
		{
			LOG_ERROR("Duplicate socketId:%d", socketId);
			_socketMap.erase(socketId);
		}
		_socketMap.insert(make_pair(socketId, socket));
	}

	void ClientMgr::erase(SOCKET socketId)
	{
		_socketMap.erase(socketId);
	}

	MySocket * ClientMgr::getSocketByFd(SOCKET socketId)
	{
		SocketMap::iterator it = _socketMap.find(socketId);
		if (it != _socketMap.end())
		{
			return it->second;
		}
		return NULL;
	}

	MySocket * ClientMgr::getSocket(int64 roleId)
	{
		ClientMap::iterator it = _clientMap.find(roleId);
		if (it != _clientMap.end())
		{
			return getSocketByFd(it->second);
		}
		return NULL;
	}

	void ClientMgr::makePair(int64 roleId, SOCKET socketId)
	{
		ClientMap::iterator it = _clientMap.find(roleId);
		if (it != _clientMap.end())
		{
			LOG_ERROR("Duplicate role:%lld", roleId);
			_clientMap.erase(roleId);
		}
		_clientMap.insert(make_pair(roleId, socketId));
	}

}//gate