#pragma once
//#ifdef LINUX

#include <lua.hpp>
#include "../../db/mongodb/MongoWrapper.h"

namespace lua {

class LuaMongoDB {
public:
	typedef std::vector<mongo::BSONObj> BSONObjList;
	//typedef std::auto_ptr<mongo::DBClientCursor> autoCursor;

	static const char className[];
	static void registerToLua(lua_State *l);

	LuaMongoDB(lua_State *l) {}

	int connect(lua_State *l);

	int ensureIndex(lua_State *l);
	int findAndModify(lua_State *l);
	int count(lua_State *l);
	int select(lua_State *l);
	int selectRaw(lua_State *l);
	int selectOne(lua_State *l);
	int selectOneRaw(lua_State *l);
	int insertProm(lua_State *l);
	int updateProm(lua_State *l);
	int deleteOneProm(lua_State *l);
	int deleteAllProm(lua_State *l);
	int insert(lua_State *l);
	int update(lua_State *l);
	int deleteOne(lua_State *l);
	int deleteAll(lua_State *l);

private:
	class Result;

	void convertElement(lua_State *l, const mongo::BSONElement &element);
	void convertBSON(lua_State *l, int pos, mongo::BSONObj &obj);
	int convertToTable(lua_State *l, int pos, mongo::DBClientCursor * cursor);

	wrapper::MongoWrapper conn_;
};

//#endif
}
