local _rpcClass = {}
_G["rpcClass"] = _rpcClass
--注册回调函数
local Handles = {}

function Handles:registHandle(handleCmd, handleFunc)
	self[handleCmd] = handleFunc
end


handles_server_json = function(src, hdl, msg, incId)
	--print("handles_server_json", src, hdl, msg)
	local msg = json.decode(msg)
	if not g_callBackMgr:handles(incId, src, msg) then
		Handles[hdl](src, msg)
	end
	return 0
end

handles_server_seri = function(src, hdl, msg, incId)
	--print("handles_fun_seri", src, hdl, msg)
	local msg = serialize.unpack(msg)
	if not g_callBackMgr:handles(incId, src, msg) then
		Handles[hdl](src, msg)
	end
	return 0
end


handles_client_json = function(roleId, handleCmd, msg, size)
	--print("handles_client_json", roleId, handleCmd, size)
	local msg = json.decode(msg)
	Handles[handleCmd](src, msg)
	
	return 0
end

handles_client_pb = function(roleId, handleCmd, msg, size)
	--print("handles_client_pb", roleId, handleCmd, size)
	local funcId = handleCmd % 65536
	local classId = (handleCmd - funcId)/ 65536
	_rpcClass[classId]:_dispatch(roleId, funcId, msg, size)
	return 0
end


g_handles = Handles
