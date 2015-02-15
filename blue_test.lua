package.cpath = package.cpath..";".."D:\\projects\\blue\\release\\?.dll"

print("starting blue")
require "util"

local nl = require "log"
local l = nl("log")
l:name()

local bl = require "blue"
local server = bl("server")

local mo = require "mongo"
local m = mo("mongo")
m:setArgs("192.168.8.223", "S1_jtxm2_5", "jtxm2", "123456")
local err = m:connect()
--if err ~= 0 then
--	print("connect error", err)
--end
-- local res = m:findOne("faction", "{}", "{}")
-- for k, v in pairs(res) do
	-- print("find one",k, json.encode(v))
-- end

local logic = require "logic"
local logic2 = logic("logic")

local gateS = require "gate"
local gate = gateS("gate")
gate:setListen("0.0.0.0", 3333)
gate:start()

server:setFrameTime(100)
server:registService(l)
server:registService(logic2)
server:registService(m)
server:registService(gate)


--
logic2:startTimer(2000)
local ret = logic2:doFile("logic_main.lua")
if ret ~= 0 then
	print("------>error", ret)
end
--]]

server:start()

gate:stop()
l = nil
--collectgarbage("collect")

print("end")