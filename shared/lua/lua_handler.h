
#pragma once 
#include <assert.h>
#include <boost/shared_ptr.hpp>
#include "../util/lua_class.h"

namespace lua
{
	template<typename T>
	class LuaHandler
	{
	public:
		//typedef SmartPtr<T> ObjPtr;

		LuaHandler(lua_State *L, bool bCreate = true) : _obj(NULL), _gc(false)
		{
			if(bCreate)
			{
				_obj = new T();
				_gc = true;
			}
		}
		~LuaHandler()
		{
			clear();
		}

		/*inline void bindObj(const ObjPtr& obj)
		{
			clear();
			_obj = obj.get();
			_gc = false;
		}*/
		inline void bindObj(T* obj, bool gc = true)
		{
			clear();
			_obj = obj;
			_gc = gc;
		}
		inline T* getObj()
		{
			assert(_obj != NULL);
			return _obj;
		}

		inline int pushObj(lua_State *L)
		{
			lua_pushlightuserdata(L, getObj());
			return 1;
		}
	protected:
		inline void clear()
		{
			if(_obj != NULL && _gc)
			{
				delete _obj;
			}
			_obj = NULL;
			_gc = false;
		}
	private:
		T* _obj;
		bool _gc;
	};
}
