
WorldMgr = oo.class(World_rpc, "WorldMgr")

function WorldMgr:__init()
	World_rpc.__init(self)
end



--------------------------------------------------以下是rpc函数

function WorldMgr:afterLoginBack(roleId, msg)
	print("WorldMgr:afterLoginBack !", json.encode(msg))
	

end




--
g_worldMgr = g_worldMgr or WorldMgr()