local _rpcClass = {}
_G["rpcClass"] = _rpcClass
--注册回调函数
local Handles = {}

function Handles:registHandle(handleCmd, handleFunc)
	self[handleCmd] = handleFunc
end


handles_server_json = function(src, fun, msg)
	print("handles_server_json", src, fun, msg)
	local msg = json.decode(msg)
	Handles[fun](src, msg)
	
	return 0
end

handles_server_seri = function(src, fun, msg)
	--print("handles_fun_seri", src, fun, msg)
	local msg = serialize.unpack(msg)
	Handles[fun](src, msg)
	
	return 0
end


handles_client_json = function(charId, handleCmd, msg, size)
	--print("handles_client_json", charId, handleCmd, size)
	local msg = json.decode(msg)
	Handles[fun](src, msg)
	
	return 0
end

handles_client_pb = function(charId, handleCmd, msg, size)
	print("handles_client_pb", charId, handleCmd, size)
	local funcId = handleCmd % 65536
	local classId = (handleCmd - funcId)/ 65536
	_rpcClass[classId]:_dispatch(charId, funcId, msg, size)
	return 0
end


g_handles = Handles
