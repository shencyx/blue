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
	self.funcList[1] = {"pluseBack", nil}
	self.funcList[2] = {"verifyBack", "account.Auth"}
	self.funcList[3] = {"getRoleBack", "account.Role"}
	self.funcList[4] = {"createRoleBack", "account.Role"}
	self.funcList[5] = {"loginBack", nil}
end

function account_oper:_dispatch(charId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[2] then
		msg, errorStr = _protobuf.decode(funcName[2], buffer, length)
	end
	if errorStr then
		print("error account_oper:", errorStr, charId, funcId, length)
	else
		self[funcName[1]](self, charId, msg)
	end
end

--------------------------------------
function account_oper:pluse(charId)
	_sendPb(nil, 0, self.operIdFix + 1, charId)
end

--------------------------------------
function account_oper:verify(charId, msg)
	pointer = _protobuf.encode("account.Verify", msg)
	_sendPb(pointer, nil, self.operIdFix + 2, charId)
end

function account_oper:verifyBack(charId, msg)
	error("account_oper:verify is not implementation!")
end

--------------------------------------
function account_oper:getRole(charId)
	_sendPb(nil, 0, self.operIdFix + 3, charId)
end

function account_oper:getRoleBack(charId, msg)
	error("account_oper:getRole is not implementation!")
end

--------------------------------------
function account_oper:createRole(charId, msg)
	pointer = _protobuf.encode("account.Role", msg)
	_sendPb(pointer, nil, self.operIdFix + 4, charId)
end

function account_oper:createRoleBack(charId, msg)
	error("account_oper:createRole is not implementation!")
end

--------------------------------------
function account_oper:login(charId)
	_sendPb(nil, 0, self.operIdFix + 5, charId)
end

--===========================================================--

account_base = oo.class(nil, "account_base")

function account_base:__init()
	self.operId = 1001
	self.operIdFix = self.operId * 65536
	_rpcClass[self.operId] = self
	_protobuf.register_file(PROTOCOL_PATH.."/logon/account.pb")
	self.funcList = {}
	self.funcList[1] = {"goddBack", nil}
	self.funcList[2] = {"hightBack", "account.Auth"}
	self.funcList[3] = {"getRoleBack", "account.Role"}
	self.funcList[4] = {"createRoleBack", "account.Role"}
	self.funcList[5] = {"loginBack", nil}
end

function account_base:_dispatch(charId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[2] then
		msg, errorStr = _protobuf.decode(funcName[2], buffer, length)
	end
	if errorStr then
		print("error account_base:", errorStr, charId, funcId, length)
	else
		self[funcName[1]](self, charId, msg)
	end
end

--------------------------------------
function account_base:godd(charId)
	_sendPb(nil, 0, self.operIdFix + 1, charId)
end

--------------------------------------
function account_base:hight(charId, msg)
	pointer = _protobuf.encode("account.Verify", msg)
	_sendPb(pointer, nil, self.operIdFix + 2, charId)
end

function account_base:hightBack(charId, msg)
	error("account_base:hight is not implementation!")
end

--------------------------------------
function account_base:getRole(charId)
	_sendPb(nil, 0, self.operIdFix + 3, charId)
end

function account_base:getRoleBack(charId, msg)
	error("account_base:getRole is not implementation!")
end

--------------------------------------
function account_base:createRole(charId, msg)
	pointer = _protobuf.encode("account.Role", msg)
	_sendPb(pointer, nil, self.operIdFix + 4, charId)
end

function account_base:createRoleBack(charId, msg)
	error("account_base:createRole is not implementation!")
end

--------------------------------------
function account_base:login(charId)
	_sendPb(nil, 0, self.operIdFix + 5, charId)
end

--===========================================================--

