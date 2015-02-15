print(">>>>>world_main")
service_name = "world"
--公共头文件
require "config.server_config"
require "config.common_include"


--功能头文件
require "world_mgr"



logicStart = function()

end

logicClose = function()

end


logicTimer = function(now)
	--print(":::::mapTimer   ")
	--comm.send("logon", "test", {good = 123})
end

print("<<<<<world_main")