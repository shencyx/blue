
--
CallBackMgr = oo.class(nil, "CallBackMgr")


function CallBackMgr:__init()
	self.callBackMap = {}
end


function CallBackMgr:regist(cbId, obj, func, args)
	assert(self.callBackMap[cbId] == nil)
	self.callBackMap[cbId] = {g_timerMgr:getTime(), obj, func, args}
end

function CallBackMgr:handles(cbId, src, msg)
	print("CallBackMgr:handles", cbId, src)
	if self.callBackMap[cbId] == nil then
		return false
	end
	local entry = self.callBackMap[cbId]
	self.callBackMap[cbId] = nil
	entry[3](entry[2], src, msg, entry[4])
	return true
end

--
g_callBackMgr = g_callBackMgr or CallBackMgr()