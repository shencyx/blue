/********************************************************************
	filename: 	Service.h
	created:	2013/07/13
	author:		shencyx@126.com
	
	purpose:	服务的基类
*********************************************************************/

#pragma once
#include <lua.hpp>
#include "Common.h"
#include "threading/RWQueue.h"
#include "util/SharedPtr.h"
#include "threading/AtomicBoolean.h"


namespace shared
{
	class Server;
	// 服务间通讯指令
	struct SERVER_DECL Instruct
	{
		Instruct(){ memset(this, 0, sizeof(*this)); }
		void mallocMsg(){
			if (msg != NULL && size > 0)
			{
				char * pBuf = new char[size];
				memcpy(pBuf, msg, size);
				msg = pBuf;
			}
		}
		void freeMsg(){ 
			if (msg != NULL)
				delete [] msg;
		}
		int32 src;
		int32 des;
		int32 type;
		uint32 session;
		int64 roleId;
		uint32 msgId;
		char handle[HANDLE_NAME_SIZE];
		void * msg;
		uint32 size;
	};
	typedef sharedPtr<Instruct> InstructPtr;

	class SERVER_DECL Service
	{
	public:
		Service(lua_State * L, const char * pName);
		virtual ~Service(){};
		virtual bool onTimer(uint64 now){ printf("service onTimer:%llu\n", now); return true; }
		virtual bool worker(InstructPtr & pIns, uint64 now){ printf("service worker:%llu\n", now); return true; }
		virtual void signal(int sig){};

		template <typename T>
		T * getService(lua_State *L, int index)
		{
			luaL_checktype(L, index, LUA_TTABLE);
			lua_getfield(L, index, "__cobj");
			T ** pService = (T**)luaL_checkudata(L, -1, T::serviceName);
			lua_pop(L, 1);
			ASSERT(*pService != NULL);
			return *pService;
		}

		void pushInstruct(InstructPtr & ins);
		InstructPtr popInstruct()
		{
			return _insQueue.frontPop();
		}

		void sendIns(const string & name, void * msg, uint32 size, int32 insType, const char * handle=NULL, uint32 session=0);
		void sendIns(int idDes, void * msg, uint32 size, int32 insType, const char * handle=NULL, uint32 session = 0);
		void sendIns(const string & name, InstructPtr pIns);
		void sendIns(int idDes, InstructPtr pIns);
		void setServer(Server * server){ _pServer = server; }
		void setId(int32 id){ _id = id; }
		int32 getPriority(){ return _priority; }
		void setPriority(int32 priority){ ASSERT((_priority = priority) > 0); }
		const string & getName(){ return _name; }
		void setName(const string & name){ _name = name; }

		Server * getServer(){ return _pServer; }
		int32 getServiceId(){ return _id; }
		const string & getServiceName(){ return _name; }

		bool isInWorkQueue(){ return _inWorkQueue.GetVal(); }
		void setInWorkQueue(bool b){ _inWorkQueue.SetVal(b); }

		uint32 getIncId(){ return ++_incId > 0 ? _incId : ++_incId; } 
	protected:
		Server * _pServer;
		Threading::AtomicBoolean _inWorkQueue;
		//bool _inWorkQueue;
	private:
		lua_State * _parentState;		// 
		int32 _id;	//自动分配的id,唯一
		int32 _priority; // 必须大于0
		uint32 _incId;	//每取一次会自增
		string _name;
		RWQueue<InstructPtr> _insQueue;
		//T *_obj;
	};

	//////////////////////////////////////////////////////////////////////////

	template <typename T>
	class ServiceLuaRegister
	{
	public:
		template <typename E>
		struct LuaAlloc
		{
			inline static E* create(lua_State* L, const char * pName)
			{
				return new E(L, pName);
			}

			inline static void destroy(lua_State* L, E* obj)
			{
				delete obj;
			}
		};

		typedef int (T::*pf_t)(lua_State*);

		ServiceLuaRegister(lua_State* L): _L(L)
		{
			lua_newtable(L);
			lua_pushcfunction(L, init);
			lua_setfield(L, -2, "__call");

			lua_pushcfunction(L, toString);
			lua_setfield(L, -2, "__tostring");

			lua_pushstring(L, T::serviceName);
			lua_setfield(L, -2, "__name");		//在metatable里

			//mt
			lua_newtable(L);
			lua_pushcfunction(L, serviceName);
			lua_setfield(L, -2, "name");		//在metatable的__index里
			lua_setfield(L, -2, "__index");

			//类的matetable
			lua_newtable(L);
			lua_pushcfunction(L, init);
			lua_setfield(L, -2, "__call");

			lua_setmetatable(L, -2);

			//userdata的metatable
			luaL_newmetatable(L, T::serviceName);
			lua_pushcfunction(L, gc);
			lua_setfield(L, -2, "__gc");
			lua_pop(L, 1);
		}
		~ServiceLuaRegister()
		{}

		static void registerClass(lua_State *L)
		{
			T::registToLua(L);
		}

		//注册函数
		template <pf_t pf>
		ServiceLuaRegister<T>& def(const char* n)
		{
			lua_State* L = _L;
			lua_getfield(L, -1, "__index");
			lua_pushcfunction(L, &fun_thunk<pf>);
			lua_setfield(L, -2, n);
			lua_pop(L, 1);

			return *this;
		}

		template <pf_t pf>
		static int fun_thunk(lua_State* L)
		{
			if(!lua_istable(L, 1))
				luaL_error(L, "%s fun_thunk is not a table", T::serviceName);

			lua_getfield(L, 1, "__cobj");
			T** ptr = (T**)lua_touserdata(L, -1);
			if(ptr == NULL || *ptr == NULL)
			{
				luaL_error(L, "%s calling method of a null", T::serviceName);
			}
			lua_pop(L, 1);

			return ((*ptr)->*pf)(L);
		}

		//是否类对象(0.2us),并取出c对象指针
		static T* checkObj(lua_State* L, int n)
		{
			luaL_checktype(L, n, LUA_TTABLE);

			int nt = lua_gettop(L);
			lua_pushvalue(L, n);
			luaL_getmetafield(L, -1, "__name");
			if(strcmp(luaL_checkstring(L, -1), T::serviceName) != 0)
			{
				luaL_error(L, "obj isnot %s", T::serviceName);
			}
			lua_pop(L, 1);
			//取指针
			lua_getfield(L, n, "__cobj");
			T** ptr = (T**)lua_touserdata(L, -1);
			lua_settop(L, nt);

			return *ptr;
		}
	private:
		static int init(lua_State* L)
		{
			int top = lua_gettop(L);
			lua_istable(L, 1);
			const char * pName = luaL_checkstring(L, 2);
			
			lua_newtable(L);
			//
			T* obj = LuaAlloc<T>::create(L, pName);
			T** ptr = (T**)lua_newuserdata(L, sizeof(T*));
			*ptr = obj;
			//设置userdata的gc
			luaL_getmetatable(L, T::serviceName);
			lua_setmetatable(L, -2);
			//
			lua_setfield(L, -2, "__cobj");
			//把类设置为对象的metatable
			lua_pushvalue(L, 1);
			lua_setmetatable(L, -2);

			return 1;
		}

		static int gc(lua_State* L)
		{
			T** ptr = (T**)lua_touserdata(L, 1);
			if(*ptr != NULL)
				LuaAlloc<T>::destroy(L, *ptr);
			*ptr = NULL;
			return 0;
		}

		static int toString(lua_State *L)
		{
			if(!lua_istable(L, 1)) return 0;

			lua_getfield(L, 1, "__cobj");
			T** ptr = (T**)lua_touserdata(L, -1);
			if(ptr != NULL)
			{
				char buff[128] = {0};
				sprintf(buff, "%p", (void*)*ptr);
				lua_pushfstring(L, "%s (%s)", T::serviceName, buff);
				return 1;
			}
			return 0;
		}

		static int serviceName(lua_State *L)
		{
			luaL_getmetafield(L, 1, "__name");
			return 1;
		}

	private:
		lua_State* _L;
	};

}//namespace shared
