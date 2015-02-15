/********************************************************************
	filename: 	cache_mgr.h
	created:	2014/01/21
	author:		shencyx@126.com
	
	purpose:	dbÊý¾Ý»º´æ
*********************************************************************/
#pragma once

#include "shared/Common.h"
#include "shared/Singleton2.h"
#include "mongo/client/dbclient.h"


namespace mongo
{
	const static string TABLE_ROLE_NAME = "role";
	const static string TABLE_ACCOUNT_NAME = "account_data";

	class MongoWrapper;

	class CacheMgr : public shared::Singleton<CacheMgr>
	{
	public:
		typedef std::map<int64, BSONObj *> IntMap;
		typedef std::map<string, BSONObj *> StrMap;

	public:
		CacheMgr(){}
		void setMongoConn(MongoWrapper * mw){ _pMongoConn = mw; }

		BSONObj * getRole(int64 roleId);
		void cacheRole(int64 roleId, BSONObj * pBo);

		BSONObj * getAccount(const string & accountName);
		void cacheAccount(const string & accountName, BSONObj * pBo);

	private:
		MongoWrapper * _pMongoConn;
		IntMap _roleMap;
		StrMap _accountMap;
	};

}