/*
 * MongoWrapper.h
 *
 *  Created on: 2011-1-12
 *      Author: root
 */
#pragma once

//#include "shared/Common.h"


#include "mongo/client/dbclient.h"
#include "mongo/client/connpool.h"

namespace mongo {

enum MongoCode {
	E_SUCCESS = 0
	, E_CONNECTION_FAILS
	, E_INVALID_ARGS
	, E_INVALID_NAMESPACE
	, E_AUTH_FAILS
	, E_INVALID_RESULT
	, E_DB_EXCEPTION
	, E_EXIST
};

class MongoWrapper {
public:
	typedef std::vector<mongo::BSONObj> BSONObjList;
	typedef std::auto_ptr<mongo::DBClientCursor> autoCursor;

	MongoWrapper();
	~MongoWrapper();

	int initConnection(const std::string &host_name, const std::string &db_name, const std::string &username, const std::string &password);

	int ensureIndex(const std::string &ns, const std::string &keyRaw, bool unique = true);

	int findAndModify(
			mongo::BSONObj &result
			, const std::string &ns
			, const char *fields
			, const char *query
			, const char *sort
			, const char *update
			, bool return_new = false
			, bool upsert = false
			, bool remove = false);
	
	//统计个数
	int count(unsigned long long &result, const std::string &ns, const std::string &query);
	int count(unsigned long long &result, const std::string &ns);

	//原始查找
	int find(autoCursor &result, const std::string &ns, const mongo::Query *query = NULL, const mongo::BSONObj *fields = NULL, int limit = 0, int skip = 0);
	
	//查找
	int find(autoCursor &result, const std::string &ns, const std::string &query, const std::string &fields, int limit = 0, int skip = 0);
	int find(autoCursor &result, const std::string &ns, const std::string &query, int limit = 0, int skip = 0);
	int find(autoCursor &result, const std::string &ns, int limit = 0, int skip = 0);
	
	//原始查找单条
	int findOne(mongo::BSONObj &result, const std::string &ns, const mongo::Query *query = NULL, const mongo::BSONObj *fields = NULL);
	//搜索单条
	int findOne(mongo::BSONObj &result, const std::string &ns, const std::string &query, const std::string &fields = "");
	
	//插入单条
	int insert(const std::string &ns, const std::string &query);

	//更新
	int update(const std::string &ns, const std::string &query, const std::string &data, 
					bool upsert = false, bool multi = false, bool raw = false);

	//删除单条
	int deleteOne(const std::string &ns, const std::string &query);

	//删除整个表数据
	int deleteAll(const std::string &ns);

	int insertMulti(const std::string &ns, const std::string &query);

	int updateMulti(const std::string &ns, const std::string &query, const std::string &data, 
		bool upsert = false, bool multi = false, bool raw = false);


	int deleteOneMulti(const std::string &ns, const std::string &query);

	int deleteAllMulti(const std::string &ns);

	int ping();
	int CHECK_PING();

protected:
	bool _run;
	time_t _pingTime;


	mongo::DBClientConnection *_conn;
private:
	int checkConnection();
	int checkThreadConnection();
	int checkNamespaceExists(const std::string &ns);
	int checkCursor(mongo::DBClientCursor *cursor);

	std::string _url;
	std::string _dbName;
	std::string _username;
	std::string _password;

	std::string _tmpS;
};

}

