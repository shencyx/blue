--package.path = package.path..";".."..\\config\\?.lua"
require "server_config"


require "account_mgr"
require "role_mgr"
require "scene_mgr"
require "world_mgr"


logicStart = function(timeMs)

end

logicClose = function(timeMs)

end

local roleCnt = 0
local roleMaxCnt = 1
logicTimer = function(timeMs)
	print(":::::logicTimer", timeMs)

	if roleCnt < roleMaxCnt then
		roleCnt = roleCnt + 1
		g_accountMgr:createRole(roleCnt)
	end
end
