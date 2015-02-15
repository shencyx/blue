local _protobuf = require "protobuf"
local _rpcClass = assert(_G["rpcClass"])
local _sendPb = assert(__sendPb)

World_rpc = oo.class(nil, "World_rpc")

function World_rpc:__init()
	self.operId = 2000
	self.operIdFix = self.operId * 65536
	_rpcClass[self.operId] = self
	_protobuf.register_file("generate/world.pb")
	self.funcList = {}
	self.funcList[1] = {"afterLoginBack", "pb.IntArray"}
end

function World_rpc:_dispatch(roleId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[2] then
		msg, errorStr = _protobuf.decode(funcName[2], buffer, length)
	end
	if errorStr then
		print("error World_rpc:", errorStr, roleId, funcId, length)
	else
		self[funcName[1]](self, roleId, msg)
	end
end

--------------------------------------
function World_rpc:afterLogin(roleId)
	_sendPb(nil, 0, self.operIdFix + 1, roleId)
end

function World_rpc:afterLoginBack(roleId, msg)
	error("World_rpc:afterLogin is not implementation!")
end

--===========================================================--

