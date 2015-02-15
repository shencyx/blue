local _protobuf = require "protobuf"
local _rpcClass = assert(_G["rpcClass"])
local _sendPb = __sendPb

account_oper = oo.class(nil, "account_oper")

function account_oper:__init()
	self.operId = 1000
	self.operIdFix = self.operId * 65536
	_rpcClass[self.operId] = self
	_protobuf.register_file(PROTOCOL_PATH.."/logon/account.pb")
	self.funcList = {}
	self.funcList[1] = {"pluse", nil, nil}
	self.funcList[2] = {"verify", "verifySend", "account.Verify"}
	self.funcList[3] = {"getRole", "getRoleSend", nil}
	self.funcList[4] = {"createRole", "createRoleSend", "account.Role"}
	self.funcList[5] = {"login", nil, nil}
end

function account_oper:_dispatch(charId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[3] then
		msg, errorStr = _protobuf.decode(funcName[3], buffer, length)
	end
	if errorStr then
		print("error account_oper:", errorStr, charId, funcId, length)
	else
		local backMsg = self[funcName[1]](self, charId, msg)
		if funcName[2] and backMsg then
			self[funcName[2]](self, charId, backMsg)
		end
	end
end

--------------------------------------
function account_oper:pluse(charId)
	error("account_oper:pluse is not implementation!")
end

--------------------------------------
function account_oper:verify(charId, msg)
	error("account_oper:verify is not implementation!")
end

function account_oper:verifySend(charId, msg)
	local pointer = _protobuf.encode("account.Auth", msg)
	_sendPb(pointer, nil, self.operIdFix + 2, charId)
end

--------------------------------------
function account_oper:getRole(charId)
	error("account_oper:getRole is not implementation!")
end

function account_oper:getRoleSend(charId, msg)
	local pointer = _protobuf.encode("account.Role", msg)
	_sendPb(pointer, nil, self.operIdFix + 3, charId)
end

--------------------------------------
function account_oper:createRole(charId, msg)
	error("account_oper:createRole is not implementation!")
end

function account_oper:createRoleSend(charId, msg)
	local pointer = _protobuf.encode("account.Role", msg)
	_sendPb(pointer, nil, self.operIdFix + 4, charId)
end

--------------------------------------
function account_oper:login(charId)
	error("account_oper:login is not implementation!")
end

--===========================================================--

account_base = oo.class(nil, "account_base")

function account_base:__init()
	self.operId = 1001
	self.operIdFix = self.operId * 65536
	_rpcClass[self.operId] = self
	_protobuf.register_file(PROTOCOL_PATH.."/logon/account.pb")
	self.funcList = {}
	self.funcList[1] = {"godd", nil, nil}
	self.funcList[2] = {"hight", "hightSend", "account.Verify"}
	self.funcList[3] = {"getRole", "getRoleSend", nil}
	self.funcList[4] = {"createRole", "createRoleSend", "account.Role"}
	self.funcList[5] = {"login", nil, nil}
end

function account_base:_dispatch(charId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[3] then
		msg, errorStr = _protobuf.decode(funcName[3], buffer, length)
	end
	if errorStr then
		print("error account_base:", errorStr, charId, funcId, length)
	else
		local backMsg = self[funcName[1]](self, charId, msg)
		if funcName[2] and backMsg then
			self[funcName[2]](self, charId, backMsg)
		end
	end
end

--------------------------------------
function account_base:godd(charId)
	error("account_base:godd is not implementation!")
end

--------------------------------------
function account_base:hight(charId, msg)
	error("account_base:hight is not implementation!")
end

function account_base:hightSend(charId, msg)
	local pointer = _protobuf.encode("account.Auth", msg)
	_sendPb(pointer, nil, self.operIdFix + 2, charId)
end

--------------------------------------
function account_base:getRole(charId)
	error("account_base:getRole is not implementation!")
end

function account_base:getRoleSend(charId, msg)
	local pointer = _protobuf.encode("account.Role", msg)
	_sendPb(pointer, nil, self.operIdFix + 3, charId)
end

--------------------------------------
function account_base:createRole(charId, msg)
	error("account_base:createRole is not implementation!")
end

function account_base:createRoleSend(charId, msg)
	local pointer = _protobuf.encode("account.Role", msg)
	_sendPb(pointer, nil, self.operIdFix + 4, charId)
end

--------------------------------------
function account_base:login(charId)
	error("account_base:login is not implementation!")
end

--===========================================================--

