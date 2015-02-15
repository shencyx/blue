#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <typeinfo>
#include <string>
#include <iostream>

#include <lua.hpp>


namespace shared
{
	int error_cb(lua_State* L);

	template <typename T>
	struct LuaAlloc
	{
		inline static T* create(lua_State* L)
		{
			return new T(L);
		}

		inline static void destroy(lua_State* L, T* obj)
		{
			delete obj;
		}
	};


	template <typename T>
	class LuaClass 
	{
	public:
		typedef int (T::*pf_t)(lua_State*);

		LuaClass(lua_State* L): _L(L)
		{
			char cc[256]={0};
			snprintf(cc, sizeof(cc)-1, "return oo.class(nil, \"%s\")", T::className);
			if(luaL_dostring(L, cc) != 0)
			{
				luaL_error(L, "%s load oo.class is error!", T::className);
			}

			lua_pushcfunction(L, init);
			lua_setfield(L, -2, "__init");

			lua_pushcfunction(L, toString);
			lua_setfield(L, -2, "__tostring");

			lua_pushcfunction(L, gc);
			lua_setfield(L, -2, "__gc");

			//_mt_
			lua_newtable(L);
			lua_pushvalue(L, -2);
			lua_setfield(L, -2, "__index");

			lua_pushcfunction(L, toString);
			lua_setfield(L, -2, "__tostring");

			lua_pushcfunction(L, gc);
			lua_setfield(L, -2, "__gc");

			lua_pushstring(L, T::className);
			lua_setfield(L, -2, "_name_");

			lua_setfield(L, -2, "_mt_");

			lua_setglobal(L, T::className);
		}
		~LuaClass()
		{}

		static void registerClass(lua_State *L)
		{
			T::registerToLua(L);
		}

		//ע�ắ��
		template <pf_t pf>
		LuaClass<T>& def(const char* n)
		{
			lua_State* L = _L;
			lua_getglobal(L, T::className);

			lua_getfield(L, -1, n);
			if(!lua_isnil(L, -1))
			{
				luaL_error(L, "%s field is exist!", n);
			}
			lua_pop(L, 1);

			lua_pushcfunction(L, &fun_thunk<pf>);
			lua_setfield(L, -2, n);
			lua_pop(L, 1);
			return *this;
		}

		//ע��meta����
		template <pf_t pf>
		LuaClass<T>& defMeta(const char* n)
		{
			lua_State* L = _L;
			lua_getglobal(L, T::className);
			lua_getfield(L, -1, "_mt_");

			lua_getfield(L, -1, n);
			if(!lua_isnil(L, -1))
			{
				luaL_error(L, "%s field is exist!", n);
			}
			lua_pop(L, 1);

			lua_pushcfunction(L, &fun_thunk<pf>);
			lua_setfield(L, -2, n);
			lua_pop(L, 2);
			return *this;
		}

		//ע�����
		LuaClass<T>& set(const char* n, int val)
		{
			lua_State* L = _L;
			lua_getglobal(L, T::className);

			lua_getfield(L, -1, n);
			if(!lua_isnil(L, -1))
			{
				luaL_error(L, "%s field is exist!", n);
			}
			lua_pop(L, 1);

			lua_pushinteger(L, val);
			lua_setfield(L, -2, n);
			lua_pop(L, 1);
			return *this;
		}

		//c����ѹջ(debug���ԣ�gc=true:2us,gc=false:1us release���ԣ�gc=true:1.1us,gc=false:0.9us)
		static int pushObj(lua_State* L, T* obj, bool gc=false) 
		{
			lua_newtable(L);
			int nt = lua_gettop(L);
			lua_getglobal(L, T::className);
			lua_getfield(L, -1, "_mt_");
			luaL_checktype(L, -1, LUA_TTABLE);
		
			lua_setmetatable(L, -3);
			push(L, obj, gc);
			lua_setfield(L, -3, "_cobj_");
			lua_settop(L, nt);

			return 0;
		}

		//�Ƿ������(0.2us),��ȡ��c����ָ��
		static T* checkObj(lua_State* L, int n)
		{
			luaL_checktype(L, n, LUA_TTABLE);

			int nt = lua_gettop(L);
			lua_pushvalue(L, n);
			lua_getmetatable(L, -1);

			lua_getfield(L, -1, "_name_");
			if(strcmp(luaL_checkstring(L, -1), T::className) != 0)
			{
				luaL_error(L, "obj isnot %s", T::className);
			}

			//ȡָ��
			lua_getfield(L, n, "_cobj_");
			T** ptr = (T**)lua_touserdata(L, -1);
			lua_settop(L, nt);

			return *ptr;
		}
	private:
		static int init(lua_State* L)
		{
			//T* obj = new T(L);
			T* obj = LuaAlloc<T>::create(L);
			push(L, obj, true);

			//lua_getmetatable(L, 1);
			//lua_setmetatable(L, -2);
			lua_setfield(L, 1, "_cobj_");

			return 0;
		}
		static int gc(lua_State* L)
		{
			if(luaL_getmetafield(L, 1, "_notgc")) 
			{
				lua_pushvalue(L, 1); 
				lua_gettable(L, -2);
				if (!lua_isnil(L, -1)) 
					return 0; 
			}

			T** ptr = (T**)lua_touserdata(L, 1);
			if(*ptr != NULL)
				LuaAlloc<T>::destroy(L, *ptr);
			*ptr = NULL;
			return 0;
		}
		static int toString(lua_State *L)
		{
			if(!lua_istable(L, 1)) return 0;

			lua_getfield(L, 1, "_cobj_");
			T** ptr = (T**)lua_touserdata(L, -1);
			if(ptr != NULL)
			{
				char buff[128] = {0};
				sprintf(buff, "%p", (void*)*ptr);
				lua_pushfstring(L, "%s (%s)", T::className, buff);
				return 1;
			}
			return 0;
		}

		template <pf_t pf>
		static int fun_thunk(lua_State* L)
		{
			if(!lua_istable(L, 1)) return 0;

			lua_getfield(L, 1, "_cobj_");
			T** ptr = (T**)lua_touserdata(L, -1);
			if(ptr == NULL || *ptr == NULL)
			{
				luaL_error(L, "%s calling method of a null", T::className);
			}
			lua_pop(L, 1);

			return ((*ptr)->*pf)(L);
		}

		//��c����pushջ
		static int push(lua_State *L, T *obj, bool gc=false) 
		{
			if(!obj) { lua_pushnil(L); return 0; }

			T** ptr = (T**)lua_newuserdata(L, sizeof(T*));
			*ptr = obj;

			int st = lua_gettop(L);
			lua_getglobal(L, T::className);
			lua_getfield(L, -1, "_mt_");
			/*if (lua_isnil(L, -1)) 
			{
			luaL_error(L, "%s missing metatable", T::className);
			return 0;
			}*/

			int mt = lua_gettop(L);
			if(gc)
			{
				lua_pushvalue(L, mt);
				lua_setmetatable(L, st);
			}
			else
			{
				subtable(L, mt, "_notgc", "k");
				lua_pushvalue(L, st);
				lua_pushboolean(L, 1);
				lua_settable(L, -3);
				lua_pop(L, 1);
			}

			lua_settop(L, st);
			return st;  
		}

		//��������
		static void weaktable(lua_State *L, const char *mode) 
		{
			lua_newtable(L);
			lua_pushvalue(L, -1);  // table is its own metatable
			lua_setmetatable(L, -2);
			lua_pushliteral(L, "__mode");
			lua_pushstring(L, mode);
			lua_settable(L, -3);   // metatable.__mode = mode
		}

		//����������(������tindex�ı��ﴴ������:{["_notgc"]:{}})
		static void subtable(lua_State *L, int tindex, const char *name, const char *mode) 
		{
			lua_pushstring(L, name);
			lua_gettable(L, tindex);

			if(lua_isnil(L, -1)) 
			{
				lua_pop(L, 1);
				weaktable(L, mode);
				lua_pushstring(L, name);
				lua_pushvalue(L, -2);
				lua_settable(L, tindex);
			}
		}
	private:
		lua_State* _L;
	};

}
