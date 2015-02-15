require "simple_loader"

if g_configMgr == nil then
	g_configMgr = {}
	g_configMgr.configList = {}
end

function g_configMgr:getConfig(fileName)
	return sele.configList[fileName]
end

function g_configMgr:loadConfig(file)
	local config = handleXmlFile(file)
	local l = string.len(file) - 4
	local s = 1
	for i = l, 1, -1 do
		if string.byte(file, i) == 47 then --47==/
			s = i + 1
			break
		end
	end
	local name = string.sub(file, s)
	assert(self.configList[name] == nil, "Duplicate config name:"..name)
	self.configList[name] = config
	--print("loadConfig", name, file)
end

--
g_configMgr:loadConfig("config/scene/battlefield.xml")
