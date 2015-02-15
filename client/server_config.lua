
SERVICE_PATH = "D:/projects/blue/Release"
COMMON_PATH = "D:/projects/blue/server/common"
--PROTOCOL_PATH = "D:/projects/blue/server/protocol"


__addPackagePath = function(path)
	package.path = package.path..";"..path.."\\?.lua"
end
__addCPath = function(path)
	package.cpath = package.cpath..";"..path.."\\?.dll"
end
__addCPath(SERVICE_PATH)
__addPackagePath(COMMON_PATH)
--__addPackagePath(PROTOCOL_PATH)


require "util"
require "lua_include"
--require "config.config_include"
--require "handles.handle_include"
require "generate.generate_include"