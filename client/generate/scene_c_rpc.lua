local _protobuf = require "protobuf"
local _rpcClass = assert(_G["rpcClass"])
local _sendPb = assert(__sendPb)

Scene_rpc = oo.class(nil, "Scene_rpc")

function Scene_rpc:__init()
	self.operId = 3000
	self.operIdFix = self.operId * 65536
	_rpcClass[self.operId] = self
	_protobuf.register_file("generate/scene.pb")
	self.funcList = {}
	self.funcList[1] = {"loginSceneBack", "scene.LoginScene"}
	self.funcList[2] = {"afterLoginBack", "pb.IntArray"}
end

function Scene_rpc:_dispatch(roleId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[2] then
		msg, errorStr = _protobuf.decode(funcName[2], buffer, length)
	end
	if errorStr then
		print("error Scene_rpc:", errorStr, roleId, funcId, length)
	else
		self[funcName[1]](self, roleId, msg)
	end
end

--------------------------------------
function Scene_rpc:loginScene(roleId)
	_sendPb(nil, 0, self.operIdFix + 1, roleId)
end

function Scene_rpc:loginSceneBack(roleId, msg)
	error("Scene_rpc:loginScene is not implementation!")
end

--------------------------------------
function Scene_rpc:afterLogin(roleId)
	_sendPb(nil, 0, self.operIdFix + 2, roleId)
end

function Scene_rpc:afterLoginBack(roleId, msg)
	error("Scene_rpc:afterLogin is not implementation!")
end

--===========================================================--

