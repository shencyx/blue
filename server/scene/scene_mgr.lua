local _mongo = require "mongo_oper"



SceneMgr = oo.class(Scene_rpc, "SceneMgr")

function SceneMgr:__init()
	Scene_rpc.__init(self)
end


function SceneMgr:loginSceneImp(src, msg, args)
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

function SceneMgr:loginScene(roleId)
	print("SceneMgr:loginScene !")
	
	local cbId = _mongo.getCache("account_data", "cen10")
	g_callBackMgr:regist(cbId, self, SceneMgr.loginSceneImp, {roleId = roleId})
end

function SceneMgr:afterLogin(roleId)
	print("SceneMgr:afterLogin !")
end



--
g_sceneMgr = g_sceneMgr or SceneMgr()