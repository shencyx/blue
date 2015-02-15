require "global_define"
local INS_TYPE = INS_TYPE

--发pb到客户端
__sendPb = function(pointer, length, handleCmd, roleId)
	my:sendIns("gate", INS_TYPE.INS_CLIENT_PB, handleCmd, pointer, length, roleId)
end


module("comm_oper", package.seeall)

--[[
	

]]
-- 

function sendJson(name, handle, msg)
	my:sendIns(name, INS_TYPE.INS_SERVER_JSON, handle, json.encode(msg))
end

function sendSeri(name, handle, msg)
	my:sendIns(name, INS_TYPE.INS_SERVER_SERI, handle, serialize.pack(msg), 0)
end

send = sendSeri


