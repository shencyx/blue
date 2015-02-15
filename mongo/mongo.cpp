#include "mongo.h"
#include "shared/GlobalDefine.h"
#include "shared/log/Log.h"
#include "shared/CmdMgr.h"
#include "cache_mgr.h"
#include "cmd_handles.h"

namespace mongo
{
	const static string STR_FIND("find");
	const static string STR_FIND_ONE("findOne");


#define CHECK_AND_PRINT(args) if(args)\
	std::cout << "\t" << #args << ": " << args << std::endl

	const char Mongo::serviceName[] = "mongo_service";

	Mongo::Mongo(lua_State * L, const char * pName):
		Service(L, pName)
	{
		CacheMgr::getSingleton().setMongoConn(&_mongoWrapper);
		shared::CmdMgr::getSingleton();
		registerHandles();
	}


	bool Mongo::worker(shared::InstructPtr & pIns, uint64 now)
	{
		printf("Mongo::worker:%lld\n", now);
		if (pIns->type == shared::INS_MONGO)
		{
			//string * s = new string("");
			//sendIns(pIns->src, pIns->msg, pIns->size, shared::INS_MONGO, NULL, pIns->session);
		}
		else if (pIns->type == shared::INS_MONGO_JSON)
		{
			shared::CmdMgr::getSingleton().handleCmdStr(this, pIns);
		}
		return true;
	}

	void Mongo::registToLua(lua_State *L)
	{
		shared::ServiceLuaRegister< Mongo > lc(L);
		lc.def< &Mongo::setArgsLua >("setArgs");
		lc.def< &Mongo::connectLua >("connect");
		lc.def< &Mongo::ensureIndex >("ensureIndex");
		lc.def< &Mongo::findAndModify >("findAndModify");
		lc.def< &Mongo::count >("count");
		lc.def< &Mongo::find >("find");
		lc.def< &Mongo::findOne >("findOne");
	}

	// ip, dbname, username, password
	int Mongo::setArgsLua(lua_State *L)
	{ 
		Mongo *pMongo = getService<Mongo>(L, 1);
		pMongo->_dbIp = luaL_checkstring(L, 2);
		pMongo->_dbName = luaL_checkstring(L, 3);
		pMongo->_username = luaL_checkstring(L, 4);
		pMongo->_password = luaL_checkstring(L, 5);

		return 0;
	}

	int Mongo::connectLua(lua_State *L)
	{
		Mongo *pMongo = getService<Mongo>(L, 1);
		int err = pMongo->_mongoWrapper.initConnection(_dbIp, _dbName, _username, _password);
		lua_pushnumber(L, err);
		return 1;
	}

	//////////////////////////////////////////////////////////////////////////


	class Mongo::Result {
	public:
		Result(lua_State *l) : l_(l), ret_num_(0) {}

		inline void PushNumber(lua_Number val) {
			++ret_num_;
			::lua_pushnumber(l_, val);
		}

		inline void PushInteger(lua_Integer val) {
			++ret_num_;
			::lua_pushinteger(l_, val);
		}

		inline void PushString(const char* val) {
			++ret_num_;
			::lua_pushstring(l_, val);
		}

		inline void PushString(const char* val, size_t length) {
			++ret_num_;
			::lua_pushlstring(l_, val, length);
		}

		inline void PushNil() {
			++ret_num_;
			::lua_pushnil(l_);

		}

		inline void PushTable(int array_size = 0, int tree_size = 0) {
			++ret_num_;
			::lua_createtable(l_, array_size, tree_size);
		}

		inline void Clear() {
			if (ret_num_)
				::lua_pop(l_, ret_num_);
			ret_num_ = 0;
		}

		inline operator int() { return ret_num_; }

	private:
		lua_State *l_;
		int ret_num_;
	};

	void Mongo::convertElement(lua_State *l, const mongo::BSONElement &element) {
		mongo::BSONType type = element.type();
		switch (type) {
		case mongo::String: {
			::lua_pushstring(l, element.String().c_str());
			break;
							}
		case mongo::NumberInt: {
			::lua_pushinteger(l, static_cast<lua_Integer>(element.Int()));
			break;
							   }
		case mongo::NumberLong: {
			::lua_pushnumber(l, static_cast<lua_Number>(element.Long()));
			break;
								}
		case mongo::NumberDouble: {
			::lua_pushnumber(l, static_cast<lua_Number>(element.numberDouble()));
			break;
								  }
		case mongo::Timestamp: {
			::lua_pushinteger(l, static_cast<lua_Integer>(element.timestampInc()));
			break;
							   }
		case mongo::Bool: {
			::lua_pushboolean(l, element.Bool() ? 1 : 0);
			break;
						  }
		case mongo::Array: {
			std::vector<mongo::BSONElement> array = element.Array();
			int index = 0;
			::lua_createtable(l, array.size(), 0);
			int top = ::lua_gettop(l);
			for (
				std::vector<mongo::BSONElement>::const_iterator begin = array.begin(), end = array.end();
				begin != end;
			++begin
				) {
					convertElement(l, *begin);
					::lua_rawseti(l, top, ++index);
			}
			break;
						   }
		case mongo::Date: {
			::lua_pushnumber(l, static_cast<lua_Number>(element.Date().millis));
			break;
						  }
		case mongo::jstNULL: {
			::lua_pushnil(l);
			break;
							 }
		case mongo::jstOID: {
			::lua_pushstring(l, element.OID().toString().c_str());
			break;
							}
		case mongo::Object: {
			mongo::BSONObj obj = element.Obj();
			::lua_createtable(l, 0, obj.nFields());
			convertBSON(l, ::lua_gettop(l), obj);
			break;
							}
		default:
			::lua_pushnil(l);
			::printf("Error: Key %s Unknow Type = %d\n", element.fieldName(), type);
			return;
		}
	}

	void Mongo::convertBSON(lua_State *l, int pos, mongo::BSONObj &obj) {
		mongo::BSONObjIterator iter = obj.begin();
		while (iter.more()) {
			mongo::BSONElement element = iter.next();
			const char* key = element.fieldName();
			convertElement(l, element);
			::lua_setfield(l, pos, key);
		}
	}

	int Mongo::convertToTable(lua_State *l, int pos, mongo::DBClientCursor * cursor) {
		int index = 0;
		while (cursor->more()) {
			mongo::BSONObj obj = cursor->next();
			::lua_createtable(l, 0, obj.nFields());
			convertBSON(l, ::lua_gettop(l), obj);
			::lua_rawseti(l, pos, ++index);
		}
		return index;
	}

	int Mongo::ensureIndex(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		int param_id = 1;	//this
		const char* ns = luaL_checkstring(l, ++param_id);
		const char* keys = luaL_checkstring(l, ++param_id);

		int args = lua_gettop(l);


		bool uniq = false;
		if (args >= ++param_id) {
			uniq = (LUA_TBOOLEAN == lua_type(l, param_id)) && 0 != lua_toboolean(l, param_id);
		}

		Result result(l);
		try {

			result.PushInteger(pMongo->_mongoWrapper.ensureIndex(ns, keys, uniq));
		} catch (mongo::DBException &ex) {
			result.Clear();
			result.PushInteger(E_DB_EXCEPTION);

			::printf("Error: MongoDB ensureIndex Exception Occur %d %s\n", ex.getCode(), ex.what());
			if (ns)
				::printf("\ns: %s\n", ns);
			if (keys)
				::printf("\nkeys: %s\n", keys);
		}
		return result;
	}

	int Mongo::find(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		int param_id = 1;	//this
		const char* table = luaL_checkstring(l, ++param_id);

		int args = lua_gettop(l);

		const char* fields = NULL;
		if (args >= ++param_id) {
			fields = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* query = NULL;
		if (args >= ++param_id) {
			query = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* orderby = NULL;
		if (args >= ++param_id) {
			orderby = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		int limit = 0;
		if (args >= ++param_id) {
			limit =  LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkinteger(l, param_id);
			if (limit < 0)
				limit = 0;
		}

		int skip = 0;
		if (args >= ++param_id) {
			skip =  LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkinteger(l, param_id);
			if (skip < 0)
				skip = 0;
		}

		const char* hint = NULL;
		if (args >= ++param_id) {
			hint = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		Result result(l);
		int top = 0;
		try {
			mongo::BSONObj filter;
			if (fields)
				filter = mongo::fromjson(fields);
			mongo::Query query_exp;
			if (query)
				query_exp = mongo::fromjson(query);
			if (orderby)
				query_exp.sort(mongo::fromjson(orderby));
			if (hint)
				query_exp.hint(hint);

			MongoWrapper::autoCursor cursor;
			int error = pMongo->_mongoWrapper.find(cursor, table, &query_exp, fields ? &filter : NULL ,limit, skip);
			if (E_SUCCESS == error) {
				result.PushTable(cursor->objsLeftInBatch());
				top = ::lua_gettop(l);
				int count = convertToTable(l, top, cursor.get());
				if (0 == count) {
					result.Clear();
					result.PushNil();
				}
			} else {
				result.PushNil();
			}
			result.PushInteger(error);
		} catch (mongo::DBException &ex) {
			if (0 != top)
				::lua_settop(l, top);
			result.Clear();
			result.PushNil();
			result.PushInteger(E_DB_EXCEPTION);

			::printf("Error: MongoDB find Exception Occur %d %s\n", ex.getCode(), ex.what());
			if (table)
				::printf("\tTable: %s\n", table);
			if (fields)
				::printf("\tFields: %s\n", fields);
			if (query)
				::printf("\tQuery: %s\n", query);
			if (orderby)
				::printf("\tOrderBy: %s\n", orderby);
			if (hint)
				::printf("\tHint: %s\n", hint);
		}
		return result;
	}


	int Mongo::findAndModify(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		int param_id = 1;	//this
		const char* table = luaL_checkstring(l, ++param_id);

		int args = lua_gettop(l);

		const char* fields = NULL;
		if (args >= ++param_id) {
			fields = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* query = NULL;
		if (args >= ++param_id) {
			query = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* sort = NULL;
		if (args >= ++param_id) {
			sort = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* update = NULL;
		if (args >= ++param_id) {
			update = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		bool return_new = false;
		if (args >= ++param_id) {
			return_new = (LUA_TBOOLEAN == lua_type(l, param_id)) && 0 != lua_toboolean(l, param_id);
		}

		bool upsert = false;
		if (args >= ++param_id) {
			upsert = (LUA_TBOOLEAN == lua_type(l, param_id)) && 0 != lua_toboolean(l, param_id);
		}

		bool remove = false;
		if (args >= ++param_id) {
			remove = (LUA_TBOOLEAN == lua_type(l, param_id)) && 0 != lua_toboolean(l, param_id);
		}

		Result result(l);
		int top = 0;
		try {
			mongo::BSONObj result_obj;
			int error = pMongo->_mongoWrapper.findAndModify(result_obj, table, fields, query, sort, update, return_new, upsert, remove);
			if (E_SUCCESS == error && !result_obj.isEmpty()) {
				result.PushTable(0, result_obj.nFields());
				top = ::lua_gettop(l);
				convertBSON(l, top, result_obj);
			} else {
				result.PushNil();
			}
			result.PushInteger(error);
		} catch (mongo::DBException &ex) {
			if (0 != top)
				::lua_settop(l, top);
			result.Clear();
			result.PushNil();
			result.PushInteger(E_DB_EXCEPTION);

			::printf("Error: MongoDB FindAndModify Exception Occur %d %s\n", ex.getCode(), ex.what());

			CHECK_AND_PRINT(table);
			CHECK_AND_PRINT(fields);
			CHECK_AND_PRINT(query);
			CHECK_AND_PRINT(sort);
			CHECK_AND_PRINT(update);
			CHECK_AND_PRINT(return_new);
			CHECK_AND_PRINT(upsert);
			CHECK_AND_PRINT(remove);
		}
		return result;
	}

	int Mongo::count(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		int param_id = 1;	//this
		const char* table = luaL_checkstring(l, ++param_id);

		int args = lua_gettop(l);

		const char* query = NULL;
		if (args >= ++param_id) {
			query = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		int limit = 0;
		if (args >= ++param_id) {
			limit =  LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkinteger(l, param_id);
			if (limit < 0)
				limit = 0;
		}

		int skip = 0;
		if (args >= ++param_id) {
			skip =  LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkinteger(l, param_id);
			if (skip < 0)
				skip = 0;
		}

		Result result(l);
		try {
			mongo::BSONObj query_exp;
			if (query)
				query_exp = mongo::fromjson(query);
			unsigned long long number = 0;
			int error = pMongo->_mongoWrapper.count(number, table, query);
			if (E_SUCCESS == error) {
				result.PushInteger((int)number);
			} else {
				result.PushNil();
			}
			result.PushInteger(error);
		} catch (mongo::DBException &ex) {
			result.Clear();
			result.PushNil();
			result.PushInteger(E_DB_EXCEPTION);

			::printf("Error: MongoDB Count Exception Occur %d %s\n", ex.getCode(), ex.what());
			if (table)
				::printf("\tTable: %s\n", table);
			if (query)
				::printf("\tQuery: %s\n", query);
		}
		return result;
	}

	//rows, e_code = MongoDB.find(db, table, fields, query, orderby, limit, skip, hint)
	int Mongo::find2(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		int param_id = 1;	//this
		const char* table = luaL_checkstring(l, ++param_id);

		int args = lua_gettop(l);

		const char* fields = NULL;
		if (args >= ++param_id) {
			fields = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* query = NULL;
		if (args >= ++param_id) {
			query = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* orderby = NULL;
		if (args >= ++param_id) {
			orderby = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		int limit = 0;
		if (args >= ++param_id) {
			limit =  LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkinteger(l, param_id);
			if (limit < 0)
				limit = 0;
		}

		int skip = 0;
		if (args >= ++param_id) {
			skip =  LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkinteger(l, param_id);
			if (skip < 0)
				skip = 0;
		}

		const char* hint = NULL;
		if (args >= ++param_id) {
			hint = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		Result result(l);
		int top = 0;
		try {
			mongo::BSONObj filter;
			if (fields)
				filter = mongo::fromjson(fields);
			mongo::Query query_exp;
			if (query)
				query_exp = mongo::fromjson(query);
			if (orderby)
				query_exp.sort(mongo::fromjson(orderby));
			if (hint)
				query_exp.hint(hint);

			MongoWrapper::autoCursor cursor;
			//int error = pMongo->_mongoWrapper.find(cursor, table, query, fields,limit, skip);
			int error = pMongo->_mongoWrapper.find(cursor, table, limit, skip);
			if (E_SUCCESS == error) {
				result.PushTable(cursor->objsLeftInBatch());
				top = ::lua_gettop(l);
				int count = convertToTable(l, top, cursor.get());
				if (0 == count) {
					result.Clear();
					result.PushNil();
				}
			} else {
				result.PushNil();
			}
			result.PushInteger(error);
		} catch (mongo::DBException &ex) {
			if (0 != top)
				::lua_settop(l, top);
			result.Clear();
			result.PushNil();
			result.PushInteger(E_DB_EXCEPTION);

			::printf("Error: MongoDB find Exception Occur %d %s\n", ex.getCode(), ex.what());
			if (table)
				::printf("\tTable: %s\n", table);
			if (fields)
				::printf("\tFields: %s\n", fields);
			if (query)
				::printf("\tQuery: %s\n", query);
			if (orderby)
				::printf("\tOrderBy: %s\n", orderby);
			if (hint)
				::printf("\tHint: %s\n", hint);
		}
		return result;
	}

	int Mongo::findOne(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		int param_id = 1;	//this
		const char* table = luaL_checkstring(l, ++param_id);

		int args = lua_gettop(l);

		const char* fields = NULL;
		if (args >= ++param_id) {
			fields = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* query = NULL;
		if (args >= ++param_id) {
			query = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* orderby = NULL;
		if (args >= ++param_id) {
			orderby = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* hint = NULL;
		if (args >= ++param_id) {
			hint = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		Result result(l);
		int top = 0;
		try {
			mongo::BSONObj filter;
			if (fields)
				filter = mongo::fromjson(fields);
			mongo::Query query_exp;
			if (query)
				query_exp = mongo::fromjson(query);
			if (orderby)
				query_exp.sort(mongo::fromjson(orderby));
			if (hint)
				query_exp.hint(hint);
			mongo::BSONObj result_obj;
			int error = pMongo->_mongoWrapper.findOne(result_obj, table, &query_exp, fields ? &filter : NULL);
			if (E_SUCCESS == error && !result_obj.isEmpty()) {
				result.PushTable(0, result_obj.nFields());
				top = ::lua_gettop(l);
				convertBSON(l, top, result_obj);
			} else {
				result.PushNil();
			}
			result.PushInteger(error);
		} catch (mongo::DBException &ex) {
			if (0 != top)
				::lua_settop(l, top);
			result.Clear();
			result.PushNil();
			result.PushInteger(E_DB_EXCEPTION);

			::printf("Error: MongoDB findOne Exception Occur %d %s\n", ex.getCode(), ex.what());
			if (table)
				::printf("\tTable: %s\n", table);
			if (fields)
				::printf("\tFields: %s\n", fields);
			if (query)
				::printf("\tQuery: %s\n", query);
			if (orderby)
				::printf("\tOrderBy: %s\n", orderby);
			if (hint)
				::printf("\tHint: %s\n", hint);
		}
		return result;
	}


	int Mongo::findOne2(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		int param_id = 1;	//this
		const char* table = luaL_checkstring(l, ++param_id);

		int args = lua_gettop(l);

		const char* fields = NULL;
		if (args >= ++param_id) {
			fields = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* query = NULL;
		if (args >= ++param_id) {
			query = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* orderby = NULL;
		if (args >= ++param_id) {
			orderby = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		const char* hint = NULL;
		if (args >= ++param_id) {
			hint = LUA_TNIL == lua_type(l, param_id) ? NULL : luaL_checkstring(l, param_id);
		}

		Result result(l);
		int top = 0;
		try {
			mongo::BSONObj filter;
			if (fields)
				filter = mongo::fromjson(fields);
			mongo::Query query_exp;
			if (query)
				query_exp = mongo::fromjson(query);
			if (orderby)
				query_exp.sort(mongo::fromjson(orderby));
			if (hint)
				query_exp.hint(hint);
			mongo::BSONObj result_obj;
			int error = pMongo->_mongoWrapper.findOne(result_obj, table, query, fields);
			if (E_SUCCESS == error && !result_obj.isEmpty()) {
				result.PushTable(0, result_obj.nFields());
				top = ::lua_gettop(l);
				convertBSON(l, top, result_obj);
			} else {
				result.PushNil();
			}
			result.PushInteger(error);
		} catch (mongo::DBException &ex) {
			if (0 != top)
				::lua_settop(l, top);
			result.Clear();
			result.PushNil();
			result.PushInteger(E_DB_EXCEPTION);

			::printf("Error: MongoDB findOne Exception Occur %d %s\n", ex.getCode(), ex.what());
			if (table)
				::printf("\tTable: %s\n", table);
			if (fields)
				::printf("\tFields: %s\n", fields);
			if (query)
				::printf("\tQuery: %s\n", query);
			if (orderby)
				::printf("\tOrderBy: %s\n", orderby);
			if (hint)
				::printf("\tHint: %s\n", hint);
		}
		return result;
	}

	int Mongo::insertProm(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		const char* table = luaL_checkstring(l, 2);
		const char* query = luaL_checkstring(l, 3);

		Result result(l);
		try {
			result.PushInteger(pMongo->_mongoWrapper.insert(table, query));
		} catch (mongo::DBException &ex) {
			result.Clear();
			result.PushInteger(E_DB_EXCEPTION);

			::printf("Error: MongoDB Insert Exception Occur %d %s\n", ex.getCode(), ex.what());
			if (table)
				::printf("\tTable: %s\n", table);
			if (query)
				::printf("\tQuery: %s\n", query);
		}
		return result;
	}

	int Mongo::updateProm(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		int param_id = 1;
		const char* table = luaL_checkstring(l, ++param_id);
		const char* query = luaL_checkstring(l, ++param_id);
		const char* data = luaL_checkstring(l, ++param_id);

		int args = lua_gettop(l);

		bool upsert = false;
		if (args >= ++param_id) {
			upsert = (LUA_TBOOLEAN == lua_type(l, param_id)) && 0 != lua_toboolean(l, param_id);
		}

		bool multi = false;
		if (args >= ++param_id) {
			multi = (LUA_TBOOLEAN == lua_type(l, param_id)) && 0 != lua_toboolean(l, param_id);
		}

		bool raw = false;
		if (args >= ++param_id) {
			raw = (LUA_TBOOLEAN == lua_type(l, param_id)) && 0 != lua_toboolean(l, param_id);
		}

		Result result(l);
		try {
			int error = pMongo->_mongoWrapper.update(table, query, data, upsert, multi, raw);
			result.PushInteger(error);
		} catch (mongo::DBException &ex) {
			result.Clear();
			result.PushInteger(E_DB_EXCEPTION);

			::printf("Error: MongoDB Update Exception Occur %d %s\n", ex.getCode(), ex.what());
			if (table)
				::printf("\tTable: %s\n", table);
			if (query)
				::printf("\tQuery: %s\n", query);
			if (data)
				::printf("\tValue: %s\n", data);
		}
		return result;
	}

	int Mongo::deleteOneProm(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		const char* table = luaL_checkstring(l, 2);
		const char* query = luaL_checkstring(l, 3);

		Result result(l);
		try {
			result.PushInteger(pMongo->_mongoWrapper.deleteOne(table, query));
		} catch (mongo::DBException &ex) {
			result.Clear();
			result.PushInteger(E_DB_EXCEPTION);

			::printf("Error: MongoDB Delete Exception Occur %d %s\n", ex.getCode(), ex.what());
			if (table)
				::printf("\tTable: %s\n", table);
			if (query)
				::printf("\tQuery: %s\n", query);
		}
		return result;
	}

	int Mongo::deleteAllProm(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		const char* table = luaL_checkstring(l, 2);

		Result result(l);
		try {
			result.PushInteger(pMongo->_mongoWrapper.deleteAll(table));
		} catch (mongo::DBException &ex) {
			result.Clear();
			result.PushInteger(E_DB_EXCEPTION);

			::printf("Error: MongoDB DelectAll Exception Occur %d %s\n", ex.getCode(), ex.what());
			if (table)
				::printf("\tTable: %s\n", table);
		}
		return result;
	}

	int Mongo::insert(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		const char* table = luaL_checkstring(l, 2);
		const char* query = luaL_checkstring(l, 3);

		Result result(l);

		result.PushInteger(pMongo->_mongoWrapper.insert(table, query));

		return result;
	}

	int Mongo::update(lua_State *l) {
		Result result(l);
		Mongo *pMongo = getService<Mongo>(l, 1);
		int param_id = 1;
		const char* table = luaL_checkstring(l, ++param_id);
		const char* query = luaL_checkstring(l, ++param_id);
		const char* data = luaL_checkstring(l, ++param_id);

		int args = lua_gettop(l);

		bool upsert = false;
		if (args >= ++param_id) {
			upsert = (LUA_TBOOLEAN == lua_type(l, param_id)) && 0 != lua_toboolean(l, param_id);
		}

		bool multi = false;
		if (args >= ++param_id) {
			multi = (LUA_TBOOLEAN == lua_type(l, param_id)) && 0 != lua_toboolean(l, param_id);
		}

		bool raw = false;
		if (args >= ++param_id) {
			raw = (LUA_TBOOLEAN == lua_type(l, param_id)) && 0 != lua_toboolean(l, param_id);
		}

		result.PushInteger(pMongo->_mongoWrapper.update(table, query, data, upsert, multi, raw));

		return result;
	}

	int Mongo::deleteOne(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		const char* table = luaL_checkstring(l, 2);
		const char* query = luaL_checkstring(l, 3);

		Result result(l);

		result.PushInteger(pMongo->_mongoWrapper.deleteOne(table, query));

		return result;
	}

	int Mongo::deleteAll(lua_State *l) {
		Mongo *pMongo = getService<Mongo>(l, 1);
		const char* table = luaL_checkstring(l, 2);

		Result result(l);

		result.PushInteger(pMongo->_mongoWrapper.deleteAll(table));

		return result;
	}
}