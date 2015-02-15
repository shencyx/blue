
SceneMgr = oo.class(Scene_rpc, "SceneMgr")

function SceneMgr:__init()
	Scene_rpc.__init(self)
end



--------------------------------------------------以下是rpc函数

function SceneMgr:loginSceneBack(roleId, msg)
	print("SceneMgr:loginScene !", json.encode(msg))
	
	g_worldMgr:afterLogin(roleId)
end

function SceneMgr:afterLoginBack(roleId)
	print("SceneMgr:afterLogin !")
end



--
g_sceneMgr = g_sceneMgr or SceneMgr()