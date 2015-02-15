print(">>>>>logon_main")
service_name = "logon"
--公共头文件
require "config.server_config"
require "config.common_include"


--功能头文件
require "account_mgr"
require "role_mgr"


logicStart = function()

end

logicClose = function()

end

--逻辑定时器
logicTimer = function(nowMs, argv)
	--print(":::::logonTimer", now, argv, self)
	g_timerMgr:onTimer(nowMs)
	--my:sendIns("log", 2, "he he")
	--g_mgr = g_mgr + 1
	--if g_mgr % 2 == 0 then
		--print("logic send mongo")
		--my:sendIns("mongo", 3, "mongo test")
		--mongo.findOne("faction", {name="ttt"}, {})
	--end
end


print("<<<<<logon_main")