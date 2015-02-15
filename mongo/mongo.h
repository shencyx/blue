/********************************************************************
	filename: 	mongo.h
	created:	2013/08/07
	author:		shencyx@126.com
	
	purpose:	mongo db service
*********************************************************************/
#pragma once

#include <lua.hpp>
#include "mongo_wrapper.h"
#include "shared/Common.h"
#include "shared/Service.h"

namespace mongo
{
	class Mongo : public shared::Service
	{
	public:
		Mongo(lua_State * L, const char * pName);
		virtual ~Mongo(){ printf("~Mongo\n"); }
		virtual bool worker(shared::InstructPtr & pIns, uint64 now);

		MongoWrapper * getMongoWrapper(){ return &_mongoWrapper; }

		// lua function
		int setArgsLua(lua_State *L);
		int connectLua(lua_State *L);

		//÷±Ω”≤È—Ø
		int ensureIndex(lua_State *l);
		int findAndModify(lua_State *l);
		int count(lua_State *l);
		int find(lua_State *l);
		int find2(lua_State *l);
		int findOne(lua_State *l);
		int findOne2(lua_State *l);
		int insertProm(lua_State *l);
		int updateProm(lua_State *l);
		int deleteOneProm(lua_State *l);
		int deleteAllProm(lua_State *l);
		int insert(lua_State *l);
		int update(lua_State *l);
		int deleteOne(lua_State *l);
		int deleteAll(lua_State *l);

		static void registToLua(lua_State *L);
		static const char serviceName[];

	private:
		class Result;

		void convertElement(lua_State *l, const mongo::BSONElement &element);
		void convertBSON(lua_State *l, int pos, mongo::BSONObj &obj);
		int convertToTable(lua_State *l, int pos, mongo::DBClientCursor * cursor);

	private:
		string _dbIp;
		string _dbName;
		string _username;
		string _password;

		MongoWrapper _mongoWrapper;

	};
}