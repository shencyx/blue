local _mongo = require "mongo_oper"



RoleMgr = oo.class(nil, "RoleMgr")

function RoleMgr:__init()
	
end


function RoleMgr:getRole(roleId)
	return {}
end


--
g_roleMgr = g_roleMgr or RoleMgr()