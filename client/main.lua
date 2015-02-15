package.cpath = package.cpath..";".."D:\\projects\\blue\\release\\?.dll"
print("starting client")
require "util"

local bl = require "blue"
local server = bl("server")


local logic = require "logic"
local logic2 = logic("client")

--[[
local gate = require "gate"
local gateS = gate("gate")
gateS:setListen("0.0.0.0", 3334)
gateS:start()
--]]

local connector = require "connector"
local conn = connector("connector")
local ret = conn:connect("127.0.0.1", 3333)
print("connect", ret)


server:setFrameTime(100)
--server:registService(gateS)
server:registService(conn)
server:registService(logic2)



logic2:startTimer(5000)
local ret = logic2:doFile("clientLogic.lua")
if ret ~= 0 then
	print("------>error", ret)
end


server:start()


print("end")