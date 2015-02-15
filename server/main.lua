--config 目录
--package.path = package.path..";".."..\\config\\?.lua"
require "config.server_config"
package.cpath = package.cpath..";"..SERVICE_PATH.."\\?.dll"
package.path = package.path..";"..COMMON_PATH.."\\?.lua"

print("starting blue")
require "util"
--
local blue = require "blue"
local server = blue("server")
--
local log = require "log"
local logS = log("log")
--
local gate = require "gate"
local gateS = gate("gate")
gateS:setListen("0.0.0.0", 3333)
gateS:start()
--
local mongo = require "mongo"
local mongS = mongo("mongo")

mongS:setArgs("192.168.8.223", "4399_S1_lwjs_4", "jtxm2", "123456")
assert(0 == mongS:connect())

--
local logic = require "logic"
local logonS = logic("logon")
local worldS = logic("world")
local sceneS = logic("scene")

--注册服务
server:setFrameTime(100)
server:registService(logS)
server:registService(gateS)
server:registService(mongS)
server:registService(logonS)
server:registService(worldS)
server:registService(sceneS)


--
logonS:startTimer(2000)
local ret = logonS:doFile(".\\logon\\logon_main.lua")
if ret ~= 0 then
	print("------>error", ret)
end
--
worldS:startTimer(1000)
local ret = worldS:doFile(".\\world\\world_main.lua")
if ret ~= 0 then
	print("------>error", ret)
end

sceneS:startTimer(3000)
local ret = sceneS:doFile(".\\scene\\scene_main.lua")
if ret ~= 0 then
	print("------>error", ret)
end


--]]

server:start()

gateS:stop()

print("end")