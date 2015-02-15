local _protobuf = require "protobuf"
local _rpcClass = assert(_G["rpcClass"])
local _sendPb = assert(__sendPb)

World_rpc = oo.class(nil, "World_rpc")

function World_rpc:__init()
	self.operId = 2000
	self.operIdFix = self.operId * 65536
	_rpcClass[self.operId] = self
	_protobuf.register_file("world/generate/world.pb")
	self.funcList = {}
	self.funcList[1] = {"afterLogin", "afterLoginSend", nil}
end

function World_rpc:_dispatch(roleId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[3] then
		msg, errorStr = _protobuf.decode(funcName[3], buffer, length)
	end
	if errorStr then
		print("error World_rpc:", errorStr, roleId, funcId, length)
	else
		local backMsg = self[funcName[1]](self, roleId, msg)
		if funcName[2] and backMsg then
			self[funcName[2]](self, roleId, backMsg)
		end
	end
end

--------------------------------------
function World_rpc:afterLogin(roleId)
	error("World_rpc:afterLogin is not implementation!")
end

function World_rpc:afterLoginSend(roleId, msg)
	local buf = _protobuf.encode("pb.IntArray", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 1, roleId)
end

--===========================================================--

