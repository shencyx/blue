#include "shared/log/Log.h"
#include "cache_mgr.h"
#include "mongo_wrapper.h"


namespace mongo
{

	BSONObj * CacheMgr::getRole(int64 roleId)
	{
		IntMap::iterator it = _roleMap.find(roleId);
		if (it != _roleMap.end())
			return it->second;
		if (_pMongoConn != NULL)
		{
			BSONObj * pBo = new BSONObj();
			mongo::Query query = QUERY( "_id" << roleId);
			if (E_SUCCESS == _pMongoConn->findOne(*pBo, TABLE_ROLE_NAME, &query))
			{
				cacheRole(roleId, pBo);
				return pBo;
			}
			else
			{
				delete pBo;
			}
		}
		return NULL;
	}

	void CacheMgr::cacheRole(int64 roleId, BSONObj * pBo)
	{
		IntMap::iterator it = _roleMap.find(roleId);
		if (it != _roleMap.end())
		{
			LOG_ERROR("duplicate cache roleId:%lld", roleId);
			_roleMap.erase(roleId);
		}
		_roleMap.insert(make_pair(roleId, pBo));
	}

	BSONObj * CacheMgr::getAccount(const string & accountName)
	{
		StrMap::iterator it = _accountMap.find(accountName);
		if (it != _accountMap.end())
			return it->second;
		if (_pMongoConn != NULL)
		{
			BSONObj * pBo = new BSONObj();
			mongo::Query query = QUERY( "account" << accountName);
			if (E_SUCCESS == _pMongoConn->findOne(*pBo, TABLE_ACCOUNT_NAME, &query))
			{
				cacheAccount(accountName, pBo);
				return pBo;
			}
			else
			{
				delete pBo;
			}
		}
		return NULL;
	}

	void CacheMgr::cacheAccount(const string & accountName, BSONObj * pBo)
	{
		StrMap::iterator it = _accountMap.find(accountName);
		if (it != _accountMap.end())
		{
			LOG_ERROR("duplicate cache account:%s", accountName.c_str());
			_accountMap.erase(accountName);
		}
		_accountMap.insert(make_pair(accountName, pBo));
	}
}