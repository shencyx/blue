
module("hot_update", package.seeall)

--模块变量管理
local local_list = {}

--配置文件是否修改管理
local config_md5_list = {}

--*******************热更新***********************
--热更新加载文件
function require_ex(path)
	package.loaded[path] = nil 
	require(path)
	--print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>hot update file:" .. path)
	engine.writeLog(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>hot update file:" .. path)
end

--创建模块变量
function create_local(key, value)
	if local_list[key] == nil then
		local_list[key] = {}
		local_list[key].value = value
	end
	return local_list[key]
end

function is_change_config_md5(file_name, file_data)
	local md5 = engine.md5(file_data)
	if config_md5_list[file_name] == md5 then
		return false
	else
		config_md5_list[file_name] = md5
		return true
	end
end

--[[function is_change_config_md5_ex(file_name)
	local file_handle = io.open(filename)
	if not file_handle then
		print("HandleXmlFile can't open the xml file, file name=", file_name)
		return false
	end

	local file_data = file_handle:read("*a")
	file_handle:close()
	if not is_change_config_md5(filename, file_data) then
		print("!!!!!!!!!!!!!!!!warning:" .. filename .. " is not change!")
		return false
	end
	return true
end]]
