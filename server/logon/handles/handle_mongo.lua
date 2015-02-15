
--[[
	处理mongo发过来的协议,如果有注册回调会先调用回调的
]]

-- cache
function g_handles.MONGO_CACHE(src, msg)
	print("g_handles:MONGO_CACHE", src, json.encode(msg))

end

-- find
function g_handles.MONGO_FIND(src, msg)
	print("g_handles:MONGO_FIND", src, json.encode(msg))

end

-- findOne
function g_handles.MONGO_FINDONE(src, msg)
	print("g_handles:MONGO_FINDONE", src, json.encode(msg))

end