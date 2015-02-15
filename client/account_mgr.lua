
AccountMgr = oo.class(Account_rpc, "AccountMgr")

function AccountMgr:__init()
	Account_rpc.__init(self)
end

function AccountMgr:createRole(roleCnt)
	local account = {}
	account.verify = "what a nice day"
	account.account = "tester" .. roleCnt
	account.serverId = 1
	account.timeStamp = g_timerMgr:getTime()
	self:verify(0, account)
end

function AccountMgr:sendVerify()
	
end


------------------------------------------以下是rpc函数
function AccountMgr:pluseBack(roleId)
	print("account_oper:pluseBack!")
end

function AccountMgr:verifyBack(roleId, msg)
	print("account_oper:verifyBack", json.encode(msg))
	if msg.code == 0 then
		g_sceneMgr:loginScene(roleId)
		self:getRole(0)
	end
end


function AccountMgr:getRoleBack(roleId, msg)
	print("account_oper:getRoleBack", json.encode(msg))

end


--
g_accountMgr = g_accountMgr or AccountMgr()