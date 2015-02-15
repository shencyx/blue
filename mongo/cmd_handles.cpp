#include "shared/Common.h"
#include "shared/GlobalDefine.h"
#include "shared/Service.h"
#include "shared/CmdMgr.h"
#include "mongo.h"
#include "mongo_wrapper.h"
#include "cache_mgr.h"
#include <assert.h>

//using namespace bson;

namespace mongo{

	static int32 handleCache(shared::Service * pService, shared::InstructPtr & pIns)
	{
		LOG_PRINT("mongo handle handleCache msg:%s", (const char*)pIns->msg);
		BSONObj msg = fromjson((const char*)pIns->msg);
		string tableName = msg.getStringField("ns");
		BSONObj *pResult = NULL;
		if (tableName.compare(TABLE_ROLE_NAME) == 0)
		{
			int64 roleId = msg["query"].numberLong();
			pResult = CacheMgr::getSingleton().getRole(roleId);

		}
		if (tableName.compare(TABLE_ACCOUNT_NAME) == 0)
		{
			string query = msg.getStringField("query");
			pResult = CacheMgr::getSingleton().getAccount(query);
		}

		if (pResult != NULL)
		{
			string s = pResult->jsonString();
			pService->sendIns(pIns->src, (void*)s.data(), s.length() + 1, shared::INS_MONGO_JSON, "MONGO_CACHE", pIns->session);
		}
		else
			LOG_ERROR("mongo cache invalid:%s, %s", tableName.c_str(), msg.getStringField("query"));

		return 0;
	}

	static int32 handleFind(shared::Service * pService, shared::InstructPtr & pIns)
	{
		LOG_PRINT("mongo handle handleFind msg:%s", (const char*)pIns->msg);
		BSONObj msg = fromjson((const char*)pIns->msg);
		try {
			MongoWrapper::autoCursor cursor;
			Query query_exp(msg.getObjectField("query"));
			BSONObj fields = msg.getObjectField("fields");
			int limit = 0;
			int skip = 0;
			int error = ((Mongo*)pService)->getMongoWrapper()->find(cursor, msg.getStringField("ns"), &query_exp, &fields, limit, skip);
			if (E_SUCCESS == error) 
			{
				//cursor->objsLeftInBatch();
				while (cursor->more()) 
				{
					BSONObj obj = cursor->next();
					//StringBuilder s;
					string s = obj.jsonString();
					pService->sendIns(pIns->src, (void*)s.data(), s.length() + 1, shared::INS_MONGO_JSON, "MONGO_FIND", pIns->session);
				}
			} 
			else 
			{
			}
		} 
		catch (DBException &ex) 
		{
			LOG_ERROR("MongoDB find Exception Occur %d %s\n", ex.getCode(), ex.what());
		}

		return 0;
	}

	static int32 handleFindOne(shared::Service * pService, shared::InstructPtr & pIns)
	{
		LOG_PRINT("mongo handle handleFindOne msg:%s", (const char*)pIns->msg);
		BSONObj msg = fromjson((const char*)pIns->msg);
		try {
			mongo::BSONObj result;
			mongo::Query query_exp(msg.getObjectField("query"));
			mongo::BSONObj fields = msg.getObjectField("fields");
			int error = ((Mongo*)pService)->getMongoWrapper()->findOne(result, msg.getStringField("ns"), &query_exp, &fields);
			if (E_SUCCESS == error) 
			{
				string s = result.jsonString();
				pService->sendIns(pIns->src, (void*)s.data(), s.length() + 1, shared::INS_MONGO_JSON, "MONGO_FINDONE", pIns->session);
			} 
			else 
			{
				LOG_ERROR("MongoDB findOne error %s %s\n", msg.getStringField("ns"), query_exp.toString().c_str());
			}
		} 
		catch (mongo::DBException &ex) 
		{
			LOG_ERROR("MongoDB findOne Exception Occur %d %s\n", ex.getCode(), ex.what());
		}

		return 0;
	}

	void registerHandles()
	{
		assert( 1 == 1);
		ASSERT(shared::CmdMgr::getSingleton().registerCmdHandle("getCache", handleCache));
		ASSERT(shared::CmdMgr::getSingleton().registerCmdHandle("find", handleFind));
		ASSERT(shared::CmdMgr::getSingleton().registerCmdHandle("findOne", handleFindOne));
	}

}//mongo