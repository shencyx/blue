
--[[
	处理gate发过来的协议
]]

function g_handles.test(src, msg)
	print("Handles:test", self, src, json.encode(msg))
	--comm_oper.send("map", "testback", "nice")
end

--
function g_handles.G2L_ACCEPT(src, msg)
	print("g_handles:G2L_ACCEPT", src, json.encode(msg))
	--comm_oper.send("map", "testback", "nice")
end