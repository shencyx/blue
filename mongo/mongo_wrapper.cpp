

#include "mongo_wrapper.h"
#include "mongo/client/constants.h"

namespace mongo{

#define CHECK_RESULT(exp) do {\
								int error = (exp);\
								if (E_SUCCESS != error)\
									return error;\
							} while (false)

#ifdef MONGODB_DEBUG
#define CHECK_ERROR_OCCUR() do {\
								mongo::BSONObj err_obj = _conn->getLastErrorDetailed();\
								if (!err_obj.isEmpty() && err_obj.hasField("code")) {\
										::printf("Error %s %d: MongoDB Error Occur %s!\n", __FILE__, __LINE__, err_obj.toString().c_str());\
										return E_DB_EXCEPTION;\
								}\
							} while (false)
#else
#define CHECK_ERROR_OCCUR() do {\
							} while (false)
#endif

#define CHECK_EMPTY_QUERY(query) do {\
									if ((query).isEmpty()) {\
										::printf("Error %s %d: MongoDB Not Access To Exce Empty Query!\n", __FILE__, __LINE__);\
										return E_INVALID_ARGS;\
									}\
								} while (false)
/*
#define CHECK_PING()  	{\
							struct timeval tv; gettimeofday(&tv, NULL); \
							if(tv.tv_sec - pingTime > 20)\
							{ \
								int ret = ping(); \
								if(ret != 0) return ret; \
							} \	
						}
*/

						
MongoWrapper::MongoWrapper() : _run(false), _conn(NULL), _tmpS("") 
{
}

MongoWrapper::~MongoWrapper() {
	if(_conn != NULL)
	{	
		delete _conn;
		_conn = NULL;
	}
	
}

int MongoWrapper::checkConnection() {
	if (NULL == _conn)
		return E_CONNECTION_FAILS;

//	if (_conn->isFailed()) {
//		::printf("Error: MongoDB Connection Fails!\n");
//		return E_CONNECTION_FAILS;
//	}
	return E_SUCCESS;
}

int MongoWrapper::checkThreadConnection() {
	//if (NULL == _threadConn)
		//return E_CONNECTION_FAILS;

	return E_SUCCESS;
}

int MongoWrapper::checkNamespaceExists(const std::string &ns) {
#ifdef MONGODB_DEBUG
	if (!_conn->exists(ns)) {
		::printf("Error: MongoDB Not Exists Table %s!\n", ns.c_str());
		return E_INVALID_NAMESPACE;
	}
#endif
	return E_SUCCESS;
}

int MongoWrapper::checkCursor(mongo::DBClientCursor *cursor) {
	if (NULL == cursor) {
		::printf("Error: MongoDB Connection Fails!\n");
		return E_CONNECTION_FAILS;
	}

	if (cursor->hasResultFlag(mongo::ResultFlag_ErrSet)) {
		return E_INVALID_RESULT;
	}
	return E_SUCCESS;
}

int MongoWrapper::initConnection(const std::string &host_name, const std::string &db_name, const std::string &username, const std::string &password) 
{
	int e_code = E_SUCCESS;
	if(_run)
	{
		return E_EXIST;
	}
	try {
		if (NULL == _conn)
			_conn = new mongo::DBClientConnection(true);

		std::string host_name1 = host_name;
		this->_conn->connect(host_name1);
		//_conn(host_name);
		

		std::string err_msg;
		
		if (!this->_conn->auth(db_name, username, password, err_msg)) {
			delete _conn;
			_conn = NULL;
			e_code = E_AUTH_FAILS;
			::printf("Error: MongoDB Auth Fails %s\n", err_msg.c_str());

		} else {
			_url = host_name;
			_dbName = db_name;
			_username = username;
			_password = password;

			//_ping_count = 0;

  			time( &_pingTime );
		}
	} catch (mongo::DBException &ex) {
		e_code = E_CONNECTION_FAILS;
		::printf("Error: MongoDB Connect Exception Occur %d %s\n", ex.getCode(), ex.what());
	}
	return e_code;
}

int MongoWrapper::ensureIndex(const std::string &table, const std::string &keyRaw, bool unique) {
	std::string ns;
	ns = _dbName + "." + table;

	int error = CHECK_PING();
	if(error != 0) return error;
	try {

		_conn->ensureIndex(ns, mongo::fromjson(keyRaw.c_str()), unique);

	} catch (mongo::DBException &ex) {
		::printf("Error: MongoDB ensureIndex Exception Occur %d %s\n", ex.getCode(), ex.what());
		error = E_DB_EXCEPTION;
	}
	return error;
}

int MongoWrapper::findAndModify(
		mongo::BSONObj &result
		, const std::string &ns
		, const char *fields
		, const char *query
		, const char *sort
		, const char *update
		, bool return_new
		, bool upsert
		, bool remove) 
{
	int error = CHECK_PING();
	if(error != 0) return error;

	try {
		CHECK_RESULT(checkConnection());
		mongo::BSONObjBuilder cmd_builder;
		cmd_builder.append("findandmodify", ns);
		if (fields)
			cmd_builder.append("fields", mongo::fromjson(fields));
		if (query)
			cmd_builder.append("query", mongo::fromjson(query));
		if (sort)
			cmd_builder.append("sort", mongo::fromjson(sort));
		if (update)
			cmd_builder.append("update", mongo::fromjson(update));
		if (return_new)
			cmd_builder.append("new", return_new);
		if (upsert)
			cmd_builder.append("upsert", upsert);
		if (remove)
			cmd_builder.append("remove", remove);
		mongo::BSONObj cmd = cmd_builder.obj();
		mongo::BSONObj info;
		if (!_conn->runCommand(_dbName, cmd, info)) {
			std::cout << info << std::endl;
			return E_INVALID_RESULT;
		}
		CHECK_ERROR_OCCUR();
		mongo::BSONElement element = info.getField("value");
		result = mongo::fromjson(element.jsonString(mongo::JS, false));	//此处是为了回避一个解析的ＢＵＧ
	} catch (mongo::DBException &ex) {
		::printf("Error: MongoDB FindAndModify Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s!\n", ns.c_str());
		return E_DB_EXCEPTION;
	}
	return E_SUCCESS;
}

int MongoWrapper::find(autoCursor &result, const std::string &table, const mongo::Query *query, const mongo::BSONObj *fields, int limit, int skip) {
	int error = CHECK_PING();
	if(error != 0) return error;
	
	std::string ns;
	ns = _dbName + "." + table;

	try {
		result = _conn->query(ns, query ? *query : mongo::Query(), limit, skip, fields);
		CHECK_ERROR_OCCUR();
		int error = checkCursor(result.get());
		if (E_SUCCESS != error) {
			if (E_INVALID_RESULT == error) {
				::printf("Error: MongoDB Invalid Query %s!\n", query ? query->toString().c_str() : "{}");
			}
			return error;
		}
	} catch (mongo::DBException &ex) {
		::printf("Error: MongoDB find Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s!\n", ns.c_str());
		if (fields)
			::printf("\tFields: %s!\n", fields->toString().c_str());
		if (query)
			::printf("\tQuery: %s!\n", query->toString().c_str());
		return E_DB_EXCEPTION;
	}
	return E_SUCCESS;
}

int MongoWrapper::find(autoCursor &result, const std::string &table, int limit, int skip) 
{
	int error = CHECK_PING();
	if(error != 0) return error;

	std::string ns;
	ns = _dbName + "." + table;

	try 
	{
		CHECK_RESULT(checkConnection());
		CHECK_RESULT(checkNamespaceExists(ns));

		result = _conn->query(ns, mongo::Query(), limit, skip);
		CHECK_ERROR_OCCUR();

		return checkCursor(result.get());
	}
	catch (mongo::DBException &ex) {
		::printf("Error: MongoDB find Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s!\n", ns.c_str());
		return E_DB_EXCEPTION;
	}
	return E_SUCCESS;
}
int MongoWrapper::find(autoCursor &result, const std::string &table, const std::string &queryRaw, 
							const std::string &fieldsRaw, int limit, int skip) 
{
	int error = CHECK_PING();
	if(error != 0) return error;

	std::string ns;
	ns = _dbName + "." + table;

	const mongo::Query query = mongo::fromjson(queryRaw);
	const mongo::BSONObj fields = mongo::fromjson(fieldsRaw);

	try {
		CHECK_RESULT(checkConnection());
		CHECK_RESULT(checkNamespaceExists(ns));

		result = _conn->query(ns, query, limit, skip, &fields);
		CHECK_ERROR_OCCUR();
		int error = checkCursor(result.get());
		if (E_SUCCESS != error) {
			if (E_INVALID_RESULT == error) 
			{
				::printf("Error: MongoDB Invalid Query %s!\n", query.toString().c_str());
			}
			return error;
		}
	} 
	catch (mongo::DBException &ex) 
	{
		::printf("Error: MongoDB find Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s!\n", ns.c_str());

		::printf("\tFields: %s!\n", fields.toString().c_str());
		::printf("\tQuery: %s!\n", query.toString().c_str());
		return E_DB_EXCEPTION;
	}
	return E_SUCCESS;
}
int MongoWrapper::find(autoCursor &result, const std::string &table, const std::string &queryRaw, int limit, int skip) 
{
	int error = CHECK_PING();
	if(error != 0) return error;

	std::string ns;
	ns = _dbName + "." + table;

	const mongo::Query query = mongo::fromjson(queryRaw);

	try {
		CHECK_RESULT(checkConnection());
		CHECK_RESULT(checkNamespaceExists(ns));

		result = _conn->query(ns, query, limit, skip);
		CHECK_ERROR_OCCUR();
		int error = checkCursor(result.get());
		if (E_SUCCESS != error) {
			if (E_INVALID_RESULT == error)
			{
				::printf("Error: MongoDB Invalid Query %s!\n", query.toString().c_str());
			}
			return error;
		}
	} 
	catch (mongo::DBException &ex) 
	{
		::printf("Error: MongoDB find Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s!\n", ns.c_str());
		::printf("\tQuery: %s!\n", query.toString().c_str());
		return E_DB_EXCEPTION;
	}
	return E_SUCCESS;
}


int MongoWrapper::findOne(mongo::BSONObj &result, const std::string &table, const mongo::Query *query, const mongo::BSONObj *fields) {
	int error = CHECK_PING();
	if(error != 0) return error;

	std::string ns;
	ns = _dbName + "." + table;

	try {
		CHECK_RESULT(checkConnection());
		CHECK_RESULT(checkNamespaceExists(ns));
		result = _conn->findOne(ns, query ? *query : mongo::Query(), fields);
		CHECK_ERROR_OCCUR();
	} catch (mongo::DBException &ex) {
		::printf("Error: MongoDB findOne Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s!\n", ns.c_str());
		if (fields)
			::printf("\tFields: %s!\n", fields->toString().c_str());
		if (query)
			::printf("\tQuery: %s!\n", query->toString().c_str());
		return E_DB_EXCEPTION;
	}
	return E_SUCCESS;
}

int MongoWrapper::findOne(mongo::BSONObj &result, const std::string &table,  const std::string &queryRaw, const std::string &fieldsRaw) 
{
	const mongo::Query query = mongo::fromjson(queryRaw);
	if (fieldsRaw.compare("") == 0)
	{
		return findOne(result, table, &query);
	}
	else
	{
		const mongo::BSONObj fields = mongo::fromjson(fieldsRaw);
		return findOne(result, table, &query, &fields);
	}
	return E_DB_EXCEPTION;
}

int MongoWrapper::insert(const std::string &table, const std::string &queryRaw) {
	int error = CHECK_PING();
	if(error != 0) return error;

	std::string ns;
	ns = _dbName + "." + table;

	

	try {
		CHECK_RESULT(checkConnection());
		CHECK_RESULT(checkNamespaceExists(ns));

		const mongo::BSONObj query = mongo::fromjson(queryRaw);

		_conn->insert(ns, query);
		CHECK_ERROR_OCCUR();
	} 
	catch (mongo::DBException &ex) 
	{
		::printf("Error: MongoDB Insert Exception Occur %d %s\n", ex.getCode(), ex.what());
		error = E_DB_EXCEPTION;
	}
	return error;
}

int MongoWrapper::insertMulti(const std::string &table, const std::string &queryRaw) {
	std::string ns;
	ns = _dbName + "." + table;

	const mongo::BSONObj query = mongo::fromjson(queryRaw);

	//mongo::ScopedDbConnection _threadConn(_url, _conn);

	int error = 0;
	try {
		CHECK_RESULT(checkThreadConnection());

		_conn->insert(ns, query);
		//_conn->insert(ns, query);
		CHECK_ERROR_OCCUR();
	} 
	catch (mongo::DBException &ex) 
	{
		::printf("Error: MongoDB Insert Exception Occur %d %s\n", ex.getCode(), ex.what());
		error = E_DB_EXCEPTION;
	}
	return error;
}

int MongoWrapper::update(const std::string &table, const std::string &queryRaw, const std::string &dataRaw, bool upsert, bool multi, bool raw) {
	int error = CHECK_PING();
	if(error != 0) return error;

	std::string ns;
	ns = _dbName + "." + table;

	const mongo::Query query = mongo::fromjson(queryRaw);
	const mongo::BSONObj data = mongo::fromjson(dataRaw);

	try {
		CHECK_RESULT(checkConnection());
		CHECK_RESULT(checkNamespaceExists(ns));
		CHECK_EMPTY_QUERY(query.obj);
		const char* field_name = data.firstElement().fieldName();
		if (!raw && field_name && '$' != field_name[0])
			_conn->update(ns, query, mongo::BSONObjBuilder().append("$set", data).obj(), upsert, multi);
		else
			_conn->update(ns, query, data, upsert, multi);
		CHECK_ERROR_OCCUR();
	} 
	catch (mongo::DBException &ex) 
	{
		error = E_DB_EXCEPTION;
		::printf("Error: MongoDB Update Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s\n", ns.c_str());
		if (!query.obj.isEmpty())
			::printf("\tQuery: %s\n", query.toString().c_str());
		if (!data.isEmpty())
			::printf("\tData: %s\n", data.toString().c_str());
	}
	return error;
}

int MongoWrapper::updateMulti(const std::string &table, const std::string &queryRaw, const std::string &dataRaw, bool upsert, bool multi, bool raw) {
	std::string ns;
	ns = _dbName + "." + table;

	const mongo::Query query = mongo::fromjson(queryRaw);
	const mongo::BSONObj data = mongo::fromjson(dataRaw);
	
	int error = 0;
	try {
		CHECK_RESULT(checkThreadConnection());
		CHECK_EMPTY_QUERY(query.obj);
		const char* field_name = data.firstElement().fieldName();

		//mongo::ScopedDbConnection conn = mongo::ScopedDbConnection( _url );
		if (!raw && field_name && '$' != field_name[0])
			_conn->update(ns, query, mongo::BSONObjBuilder().append("$set", data).obj(), upsert, multi);
			
		else
			_conn->update(ns, query, data, upsert, multi);
			
		CHECK_ERROR_OCCUR();
	} 
	catch (mongo::DBException &ex) 
	{
		error = E_DB_EXCEPTION;
		::printf("Error: MongoDB Update Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s\n", ns.c_str());
		if (!query.obj.isEmpty())
			::printf("\tQuery: %s\n", query.toString().c_str());
		if (!data.isEmpty())
			::printf("\tData: %s\n", data.toString().c_str());
	}
	return error;
}

int MongoWrapper::deleteOne(const std::string &table, const std::string &queryRaw) {
	int error = CHECK_PING();
	if(error != 0) return error;

	std::string ns;
	ns = _dbName + "." + table;

	const mongo::Query query = mongo::fromjson(queryRaw);

	try {
		CHECK_RESULT(checkConnection());
		CHECK_RESULT(checkNamespaceExists(ns));
		CHECK_EMPTY_QUERY(query.obj);
		_conn->remove(ns, query);
		CHECK_ERROR_OCCUR();
	}
	catch (mongo::DBException &ex) 
	{
		error = E_DB_EXCEPTION;
		::printf("Error: MongoDB Delete Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s\n", ns.c_str());
		if (!query.obj.isEmpty())
			::printf("\tQuery: %s\n", query.toString().c_str());
	}
	return error;
}

int MongoWrapper::deleteOneMulti(const std::string &table, const std::string &queryRaw) {
	std::string ns;
	ns = _dbName + "." + table;

	const mongo::Query query = mongo::fromjson(queryRaw);
	int error = 0;

	try {
		CHECK_RESULT(checkThreadConnection());
		CHECK_EMPTY_QUERY(query.obj);

		_conn->remove(ns, query);

		CHECK_ERROR_OCCUR();
	}
	catch (mongo::DBException &ex) 
	{
		error = E_DB_EXCEPTION;
		::printf("Error: MongoDB Delete Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s\n", ns.c_str());
		if (!query.obj.isEmpty())
			::printf("\tQuery: %s\n", query.toString().c_str());
	}
	return error;
}

int MongoWrapper::count(unsigned long long &result, const std::string &table, 
									const std::string &queryRaw) 
{
	int error = CHECK_PING();
	if(error != 0) return error;

	std::string ns;
	ns = _dbName + "." + table;

	const mongo::BSONObj query = mongo::fromjson(queryRaw);

	try {
		CHECK_RESULT(checkConnection());
		CHECK_RESULT(checkNamespaceExists(ns));
		//result = _conn->count(ns, query, 0, limit, skip);
		result = _conn->count(ns, query);
		CHECK_ERROR_OCCUR();
	} 
	catch (mongo::DBException &ex) 
	{
		error = E_DB_EXCEPTION;
		::printf("Error: MongoDB Count Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s\n", ns.c_str());
		if (!query.isEmpty())
			::printf("\tQuery: %s\n", query.toString().c_str());
	}
	return error;
}
int MongoWrapper::count(unsigned long long &result, const std::string &table) 
{
	int error = CHECK_PING();
	if(error != 0) return error;

	std::string ns;
	ns = _dbName + "." + table;

	try 
	{
		CHECK_RESULT(checkConnection());
		CHECK_RESULT(checkNamespaceExists(ns));
		//result = _conn->count(ns, query, 0, limit, skip);
		result = _conn->count(ns);
		CHECK_ERROR_OCCUR();
	} 
	catch (mongo::DBException &ex) 
	{
		error = E_DB_EXCEPTION;
		::printf("Error: MongoDB Count Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (!ns.empty())
			::printf("\tTable: %s\n", ns.c_str());
	}
	return error;
}

int MongoWrapper::deleteAll(const std::string &table) {
	int error = CHECK_PING();
	if(error != 0) return error;

	std::string ns;
	ns = _dbName + "." + table;

	try {
		CHECK_RESULT(checkConnection());
		CHECK_RESULT(checkNamespaceExists(ns));
		mongo::BSONObj query;
		_conn->remove(ns, query);
		CHECK_ERROR_OCCUR();
	} 
	catch (mongo::DBException &ex) 
	{
		error = E_DB_EXCEPTION;
		::printf("Error: MongoDB DeleteAll Exception Occur %d %s\n", ex.getCode(), ex.what());
	}
	return error;
}

int MongoWrapper::deleteAllMulti(const std::string &table) {
	int error = 0;

	std::string ns;
	ns = _dbName + "." + table;

	try {
		CHECK_RESULT(checkThreadConnection());
		mongo::BSONObj query;
		_conn->remove(ns, query);
		CHECK_ERROR_OCCUR();
	} 
	catch (mongo::DBException &ex) 
	{
		error = E_DB_EXCEPTION;
		::printf("Error: MongoDB DeleteAll Exception Occur %d %s\n", ex.getCode(), ex.what());
	}
	return error;
}


int MongoWrapper::CHECK_PING() 
{
	time_t rawtime; 
	time( &rawtime ); 

	if(rawtime - _pingTime > 20) 
	{ 
		time( &_pingTime ); 
		int ret = ping(); 
		if(ret != 0) return ret; 
	} 

	return 0;
}


int MongoWrapper::ping() {
	if (_url.empty() || NULL == _conn)
		return E_CONNECTION_FAILS;

	try {
		mongo::BSONObjBuilder cmd_builder;
		cmd_builder.append("ping", 1);
		mongo::BSONObj cmd = cmd_builder.obj();
		mongo::BSONObj info;
		if (!_conn->runCommand(_dbName, cmd, info)) {
			std::cout << info << std::endl;
			return E_INVALID_RESULT;
		}

		CHECK_ERROR_OCCUR();
		return E_SUCCESS;
	}
	catch (mongo::DBException &ex) 
	{
		::printf("Error: MongoDB ping Exception Occur %d %s\n", ex.getCode(), ex.what());
		if (_conn->isFailed())
			return E_CONNECTION_FAILS;
	}

	return E_CONNECTION_FAILS;
}

}//mongo

