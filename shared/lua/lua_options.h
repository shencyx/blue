
#pragma once
#include "../util/options.h"
#include "../util/lua_class.h"

namespace lua
{
	/*struct Option
	{
		std::string binPath;
		std::string binDir;
		std::string binName;

		std::string scriptPath;
		std::string scriptDir;
		std::string scriptName;
	};*/

	bool optionInit(lua_State* L, const util::Option& op);
}
