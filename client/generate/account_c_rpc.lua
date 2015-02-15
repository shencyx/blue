local _protobuf = require "protobuf"
local _rpcClass = assert(_G["rpcClass"])
local _sendPb = assert(__sendPb)

Account_rpc = oo.class(nil, "Account_rpc")

function Account_rpc:__init()
	self.operId = 1000
	self.operIdFix = self.operId * 65536
	_rpcClass[self.operId] = self
	_protobuf.register_file("generate/account.pb")
	self.funcList = {}
	self.funcList[1] = {"pluseBack", nil}
	self.funcList[2] = {"verifyBack", "account.Auth"}
	self.funcList[3] = {"getRoleBack", "account.Role"}
	self.funcList[4] = {"createRoleBack", "account.Role"}
	self.funcList[5] = {"loginBack", nil}
end

function Account_rpc:_dispatch(roleId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[2] then
		msg, errorStr = _protobuf.decode(funcName[2], buffer, length)
	end
	if errorStr then
		print("error Account_rpc:", errorStr, roleId, funcId, length)
	else
		self[funcName[1]](self, roleId, msg)
	end
end

--------------------------------------
function Account_rpc:pluse(roleId)
	_sendPb(nil, 0, self.operIdFix + 1, roleId)
end

--------------------------------------
function Account_rpc:verify(roleId, msg)
	buf = _protobuf.encode("account.Verify", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 2, roleId)
end

function Account_rpc:verifyBack(roleId, msg)
	error("Account_rpc:verify is not implementation!")
end

--------------------------------------
function Account_rpc:getRole(roleId)
	_sendPb(nil, 0, self.operIdFix + 3, roleId)
end

function Account_rpc:getRoleBack(roleId, msg)
	error("Account_rpc:getRole is not implementation!")
end

--------------------------------------
function Account_rpc:createRole(roleId, msg)
	buf = _protobuf.encode("account.Role", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 4, roleId)
end

function Account_rpc:createRoleBack(roleId, msg)
	error("Account_rpc:createRole is not implementation!")
end

--------------------------------------
function Account_rpc:login(roleId)
	_sendPb(nil, 0, self.operIdFix + 5, roleId)
end

--===========================================================--

AccountTest_rpc = oo.class(nil, "AccountTest_rpc")

function AccountTest_rpc:__init()
	self.operId = 1001
	self.operIdFix = self.operId * 65536
	_rpcClass[self.operId] = self
	_protobuf.register_file("generate/account.pb")
	self.funcList = {}
	self.funcList[1] = {"goddBack", nil}
	self.funcList[2] = {"hightBack", "account.Auth"}
	self.funcList[3] = {"getRoleBack", "account.Role"}
	self.funcList[4] = {"createRoleBack", "account.Role"}
	self.funcList[5] = {"loginBack", nil}
end

function AccountTest_rpc:_dispatch(roleId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[2] then
		msg, errorStr = _protobuf.decode(funcName[2], buffer, length)
	end
	if errorStr then
		print("error AccountTest_rpc:", errorStr, roleId, funcId, length)
	else
		self[funcName[1]](self, roleId, msg)
	end
end

--------------------------------------
function AccountTest_rpc:godd(roleId)
	_sendPb(nil, 0, self.operIdFix + 1, roleId)
end

--------------------------------------
function AccountTest_rpc:hight(roleId, msg)
	buf = _protobuf.encode("account.Verify", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 2, roleId)
end

function AccountTest_rpc:hightBack(roleId, msg)
	error("AccountTest_rpc:hight is not implementation!")
end

--------------------------------------
function AccountTest_rpc:getRole(roleId)
	_sendPb(nil, 0, self.operIdFix + 3, roleId)
end

function AccountTest_rpc:getRoleBack(roleId, msg)
	error("AccountTest_rpc:getRole is not implementation!")
end

--------------------------------------
function AccountTest_rpc:createRole(roleId, msg)
	buf = _protobuf.encode("account.Role", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 4, roleId)
end

function AccountTest_rpc:createRoleBack(roleId, msg)
	error("AccountTest_rpc:createRole is not implementation!")
end

--------------------------------------
function AccountTest_rpc:login(roleId)
	_sendPb(nil, 0, self.operIdFix + 5, roleId)
end

--===========================================================--

