

ObjMgr = oo.class(nil, "ObjMgr")

function ObjMgr:__init()
	self.roleMap = {}	--key:orleId value:ObjHuman
end


function ObjMgr:loginScene(roleId, humanObj)
	assert(self.roleMap[roleId] == nil)
	self.roleMap[roleId] = humanObj
end


--
g_objMgr = g_objMgr or ObjMgr()