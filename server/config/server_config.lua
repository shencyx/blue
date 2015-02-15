
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
if service_name == "logon" then
	__addPackagePath("./logon")
elseif service_name == "world" then
	__addPackagePath("./world")
elseif service_name == "scene" then
	__addPackagePath("./scene")
end
