
require "config.server_config"
require "config.common_include"



print("in logon")

mongo = require "mongo_oper"
require "account_mgr"


local what = 5
for i = 0, what do
	print("just a test 222", i)
end
g_mgr = 0
g_accountMgr = AccountMgr()

logicTimer = function(now, argv)
	--print(":::::logonTimer", now, argv, self)
	--my:sendIns("log", 2, "he he")
	g_mgr = g_mgr + 1
	if g_mgr % 2 == 0 then
		--print("logic send mongo")
		--my:sendIns("mongo", 3, "mongo test")
		--mongo.findOne("faction", {name="ttt"}, {})
	end
end

print("end logon")