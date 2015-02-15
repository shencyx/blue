require "global_define"
local INS_TYPE = INS_TYPE
local INS_CLIENT_PB = INS_TYPE.INS_CLIENT_PB
local INS_SERVER_JSON = INS_TYPE.INS_SERVER_JSON
local INS_SERVER_SERI = INS_TYPE.INS_SERVER_SERI


--发pb到客户端
__sendPb = function(buf, len, handleCmd, roleId)
	my:sendIns("gate", INS_CLIENT_PB, handleCmd, buf, len, roleId)
end




--[[
	

]]
module("comm_oper", package.seeall)

function sendJson(name, handle, msg)
	return my:sendIns(name, INS_SERVER_JSON, handle, json.encode(msg))
end

function sendSeri(name, handle, msg)
	return my:sendIns(name, INS_SERVER_SERI, handle, serialize.pack(msg), 0)
end

send = sendSeri


