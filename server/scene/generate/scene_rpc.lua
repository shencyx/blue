local _protobuf = require "protobuf"
local _rpcClass = assert(_G["rpcClass"])
local _sendPb = assert(__sendPb)

Scene_rpc = oo.class(nil, "Scene_rpc")

function Scene_rpc:__init()
	self.operId = 3000
	self.operIdFix = self.operId * 65536
	_rpcClass[self.operId] = self
	_protobuf.register_file("scene/generate/scene.pb")
	self.funcList = {}
	self.funcList[1] = {"loginScene", "loginSceneSend", nil}
	self.funcList[2] = {"afterLogin", "afterLoginSend", nil}
end

function Scene_rpc:_dispatch(roleId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[3] then
		msg, errorStr = _protobuf.decode(funcName[3], buffer, length)
	end
	if errorStr then
		print("error Scene_rpc:", errorStr, roleId, funcId, length)
	else
		local backMsg = self[funcName[1]](self, roleId, msg)
		if funcName[2] and backMsg then
			self[funcName[2]](self, roleId, backMsg)
		end
	end
end

--------------------------------------
function Scene_rpc:loginScene(roleId)
	error("Scene_rpc:loginScene is not implementation!")
end

function Scene_rpc:loginSceneSend(roleId, msg)
	local buf = _protobuf.encode("scene.LoginScene", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 1, roleId)
end

--------------------------------------
function Scene_rpc:afterLogin(roleId)
	error("Scene_rpc:afterLogin is not implementation!")
end

function Scene_rpc:afterLoginSend(roleId, msg)
	local buf = _protobuf.encode("pb.IntArray", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 2, roleId)
end

--===========================================================--

