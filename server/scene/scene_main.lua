print(">>>>>scene_main")
service_name = "scene"
--公共头文件
require "config.server_config"
require "config.common_include"


--功能头文件
require "scene_mgr"
require "obj.obj_include"


logicStart = function()

end

logicClose = function()

end


logicTimer = function(now)
	--print(":::::mapTimer   ")
	--comm.send("logon", "test", {good = 123})
end

print("<<<<<scene_main")