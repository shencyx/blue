require "global_define"
local INS_TYPE = INS_TYPE

module("log_oper", package.seeall)

--[[
	

]]
-- 
function send(msg)
	if type(msg) ~= "string" then
		msg = json.encode(msg)
	end
	my:sendIns("log", INS_TYPE.INS_LOG, nil, msg)
end
