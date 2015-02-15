
#pragma once
#include "../util/lua_class.h"
#include "../util/singleton.h"

namespace lua
{
	class LuaConfig : public util::Singleton<LuaConfig>
	{
	public:
		LuaConfig();
		~LuaConfig();

		void openLua(lua_State* L);
		void closeLua();
		inline bool isFlag(){return _flag;}
		inline lua_State* getLuaState(){return _L;}

		void utilRegisterTolua();
	private:
		lua_State* _L;
		bool _flag;
	};
}
