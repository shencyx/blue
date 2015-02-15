local _mongo = require "mongo_oper"



WorldMgr = oo.class(World_rpc, "WorldMgr")

function WorldMgr:__init()
	World_rpc.__init(self)
end


function WorldMgr:loginSceneImp(src, msg, args)
	print("loginSceneImp", src, json.encode(msg))
	
	local human = {}
	g_objMgr:loginScene(msg.char_id, human)
	
	local msgBack = {}
	msgBack.roleId = msg.char_id;
	msgBack.name = "cen";
	msgBack.occ = 4;
	msgBack.gender = 3;
	msgBack.camp = 2;
	msgBack.level = 1;
	self:loginSceneSend(msg.char_id, msgBack)
	

end

--------------------------------------------------以下是rpc函数

function WorldMgr:afterLogin(roleId)
	print("WorldMgr:afterLogin !", roleId)
	--return {122,113,114,115}
end



--
g_worldMgr = g_worldMgr or WorldMgr()