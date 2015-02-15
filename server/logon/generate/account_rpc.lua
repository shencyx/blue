local _protobuf = require "protobuf"
local _rpcClass = assert(_G["rpcClass"])
local _sendPb = assert(__sendPb)

Account_rpc = oo.class(nil, "Account_rpc")

function Account_rpc:__init()
	self.operId = 1000
	self.operIdFix = self.operId * 65536
	_rpcClass[self.operId] = self
	_protobuf.register_file("logon/generate/account.pb")
	self.funcList = {}
	self.funcList[1] = {"pluse", nil, nil}
	self.funcList[2] = {"verify", "verifySend", "account.Verify"}
	self.funcList[3] = {"getRole", "getRoleSend", nil}
	self.funcList[4] = {"createRole", "createRoleSend", "account.Role"}
	self.funcList[5] = {"login", nil, nil}
end

function Account_rpc:_dispatch(roleId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[3] then
		msg, errorStr = _protobuf.decode(funcName[3], buffer, length)
	end
	if errorStr then
		print("error Account_rpc:", errorStr, roleId, funcId, length)
	else
		local backMsg = self[funcName[1]](self, roleId, msg)
		if funcName[2] and backMsg then
			self[funcName[2]](self, roleId, backMsg)
		end
	end
end

--------------------------------------
function Account_rpc:pluse(roleId)
	error("Account_rpc:pluse is not implementation!")
end

--------------------------------------
function Account_rpc:verify(roleId, msg)
	error("Account_rpc:verify is not implementation!")
end

function Account_rpc:verifySend(roleId, msg)
	local buf = _protobuf.encode("account.Auth", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 2, roleId)
end

--------------------------------------
function Account_rpc:getRole(roleId)
	error("Account_rpc:getRole is not implementation!")
end

function Account_rpc:getRoleSend(roleId, msg)
	local buf = _protobuf.encode("account.Role", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 3, roleId)
end

--------------------------------------
function Account_rpc:createRole(roleId, msg)
	error("Account_rpc:createRole is not implementation!")
end

function Account_rpc:createRoleSend(roleId, msg)
	local buf = _protobuf.encode("account.Role", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 4, roleId)
end

--------------------------------------
function Account_rpc:login(roleId)
	error("Account_rpc:login is not implementation!")
end

--===========================================================--

AccountTest_rpc = oo.class(nil, "AccountTest_rpc")

function AccountTest_rpc:__init()
	self.operId = 1001
	self.operIdFix = self.operId * 65536
	_rpcClass[self.operId] = self
	_protobuf.register_file("logon/generate/account.pb")
	self.funcList = {}
	self.funcList[1] = {"godd", nil, nil}
	self.funcList[2] = {"hight", "hightSend", "account.Verify"}
	self.funcList[3] = {"getRole", "getRoleSend", nil}
	self.funcList[4] = {"createRole", "createRoleSend", "account.Role"}
	self.funcList[5] = {"login", nil, nil}
end

function AccountTest_rpc:_dispatch(roleId, funcId, buffer, length)
	local funcName = self.funcList[funcId]
	local msg, errorStr = nil, nil
	if funcName[3] then
		msg, errorStr = _protobuf.decode(funcName[3], buffer, length)
	end
	if errorStr then
		print("error AccountTest_rpc:", errorStr, roleId, funcId, length)
	else
		local backMsg = self[funcName[1]](self, roleId, msg)
		if funcName[2] and backMsg then
			self[funcName[2]](self, roleId, backMsg)
		end
	end
end

--------------------------------------
function AccountTest_rpc:godd(roleId)
	error("AccountTest_rpc:godd is not implementation!")
end

--------------------------------------
function AccountTest_rpc:hight(roleId, msg)
	error("AccountTest_rpc:hight is not implementation!")
end

function AccountTest_rpc:hightSend(roleId, msg)
	local buf = _protobuf.encode("account.Auth", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 2, roleId)
end

--------------------------------------
function AccountTest_rpc:getRole(roleId)
	error("AccountTest_rpc:getRole is not implementation!")
end

function AccountTest_rpc:getRoleSend(roleId, msg)
	local buf = _protobuf.encode("account.Role", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 3, roleId)
end

--------------------------------------
function AccountTest_rpc:createRole(roleId, msg)
	error("AccountTest_rpc:createRole is not implementation!")
end

function AccountTest_rpc:createRoleSend(roleId, msg)
	local buf = _protobuf.encode("account.Role", msg)
	_sendPb(buf, string.len(buf), self.operIdFix + 4, roleId)
end

--------------------------------------
function AccountTest_rpc:login(roleId)
	error("AccountTest_rpc:login is not implementation!")
end

--===========================================================--

