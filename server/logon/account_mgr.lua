local _mongo = require "mongo_oper"



AccountMgr = oo.class(Account_rpc, "AccountMgr")

function AccountMgr:__init()
	Account_rpc.__init(self)
end


function AccountMgr:loadRole(src, msg, args)
	--print("loadRole", src, json.encode(msg))
	
	
	local msgBack = {}
	msgBack.roleId = msg.char_id;
	msgBack.fd = args.fd;
	msgBack.code = 0;
	msgBack.state = 0;
	comm_oper.sendJson("gate", "L2G_LOGIN", msgBack)
	--
	self:verifySend(msg.char_id, {code = 0})
	

end

--------------------------------------------------以下是rpc函数
function AccountMgr:pluse(roleId)
	print("AccountMgr:pluse!")
	
	return {code = 123}
end

function AccountMgr:verify(roleId, msg)
	--print("AccountMgr:verify!", roleId, json.encode(msg))
	
	local cbId = _mongo.getCache("account_data", "cen10")
	g_callBackMgr:regist(cbId, self, AccountMgr.loadRole, {fd = roleId})
	
	--return {code = code} 这里不能直接返回，这里的roleId是fd
end

function AccountMgr:getRole(roleId, msg)
	
	local ret = {}
	ret.roleId = roleId
	ret.name = "shen"
	ret.occ = 1
	ret.gender = 2
	ret.camp = 3
	ret.level = 4
	return ret
end


--
g_accountMgr = g_accountMgr or AccountMgr()