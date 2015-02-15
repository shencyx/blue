

table.size = function(o) 
	--[[if type(o) ~= "table" then
		local debug = Debug(g_debug_log)
		debug:trace("Scene_mgr:get_scene_by_obj")
	end]]

	local ret = 0
	local k,v
	while true do
		k, v = next(o, k)
		if k == nil then break end
		ret = ret + 1
	end
	return ret
end

table.copy = function(src, dst)
	if src == nil then
		debug.traceback("Scene_mgr:get_scene_by_obj")
	end

	dst = dst or {}
	local k, v
	for k,v in pairs(src) do
		if type(v) == "table" then
			dst[k] = {}
			table.copy(src[k], dst[k])
		else
			dst[k] = v
		end
		
	end
	return dst
end

table.clear = function(o)
	local k, v
	while true do
		k, v = next(o, k)
		if k == nil then return end
		o[k] = nil
	end
end

table.toarray = function(o)
	local ret = {}
	local i = 1
	for k,v in pairs(o) do
		ret[i] = v
		i = i + 1
	end
	return ret
end

table.freeindex = function(o)
	local i = 1
	while true do
		if not o[i] then
			return i
		end
		i = i + 1
	end
end

table.duplicate = function(ori_tab, new_tab)
    if (type(ori_tab) ~= "table") then
        return nil
    end
	new_tab = new_tab or {}
    for i,v in pairs(ori_tab) do
        local vtyp = type(v)
        if (vtyp == "table") then
            new_tab[i] = table.duplicate(v)
        elseif (vtyp == "thread") then
            -- TODO: dup or just point to?
            new_tab[i] = v
        elseif (vtyp == "userdata") then
            -- TODO: dup or just point to?
            new_tab[i] = v
        else
            new_tab[i] = v
        end
    end
    return new_tab
end

--判定表是否为空
--[[function table_is_empty(t)

	return _G.next(t) == nil
end]]

table.isEmpty = function(t)
	return _G.next(t) == nil
end

table.print = function(t, s, c)
	c = c or (s or "\t")
	s = s or ""
	for k, v in pairs(t or {}) do
		print(s, k, v, type(v))
		if type(v) == "table" then
			table.print(v, s..c, c)
		end
	end
end


function changeFromTable(tableName, t, tRpc)
	if type(tableName) == "string" then
		tRpc:set_sname(tableName)
	elseif type(tableName) == "number" then
		tRpc:set_iname(tableName)
	else
		print("Error tableName is nil!")
	end
	for k, v in pairs(t) do
		local valueType = type(v)
		local keyType = type(k)
		if valueType == "string" then
			if keyType == "number" then
				local ivalue = tRpc:add_ivalue()
				ivalue:set_id(k)
				ivalue:set_value(v)
			elseif keyType == "string" then
				local kstring = tRpc:add_kstring()
				kstring:set_key(k)
				kstring:set_value(v)
			else
				print("Error tableKey is illegality!")
			end
		elseif valueType == "number" then
			if keyType == "number" then
				local iint = tRpc:add_iint()
				iint:set_key(k)
				iint:set_value(v)
			elseif keyType == "string" then
				local kvalue = tRpc:add_kvalue()
				kvalue:set_key(k)
				kvalue:set_value(v)
			else
				print("Error tableKey is illegality!")
			end
		elseif valueType == "table" then
			local tableRpc = tRpc:add_t()
			changeFromTable(k, v, tableRpc)
		else
			print("Error tableValue is illegality!")
		end
	end
end

function changeFromRpc(tRpc)
	local tName
	local t = {}
	if tRpc:has_sname() then
		tName = tRpc:sname()
	elseif tRpc:has_iname() then
		tName = tRpc:iname()
	else
		print("Error RpctableName is nil!")
	end

	for i = 0, tRpc:kvalue_size() - 1 do
		local kvalue = tRpc:kvalue(i)
		t[kvalue:key()] = kvalue:value()
	end
	for i = 0, tRpc:ivalue_size() - 1 do
		local ivalue = tRpc:ivalue(i)
		t[ivalue:id()] = ivalue:value()
	end
	for i = 0, tRpc:kstring_size() - 1 do
		local kstring = tRpc:kstring(i)
		t[kstring:key()] = kstring:value()
	end
	for i = 0, tRpc:iint_size() - 1 do
		local iint = tRpc:iint(i)
		t[iint:key()] = iint:value()
	end

	for i = 0, tRpc:t_size() - 1 do
		local talbevalue = tRpc:t(i)
		local tmpTable, tableName = changeFromRpc(talbevalue)
		t[tableName] = tmpTable
	end 
	return t, tName 
end

-- 打印table 注意：不要打印循环嵌套表
table.print_t = function(value,fmt,tabnum)
	local num = tabnum or 1
	local fmt = fmt or '    '
	if type(value) =="table" then
		local left = fmt .. '{'
		local right = fmt .. '}' ..','
		local newfmt = fmt .. '    '
		if num<= 1 then
			left = '{'
			right = '}'
			newfmt = fmt
		end

		print (left)
		for k,v in pairs(value) do
			if type(k) == "string" then
				k = '\"' .. k .. '\"'
			end
			
			if type(v) == "table" then
				print(newfmt.."["..k.."]" .. " = ")
				table.print_t(v,newfmt,num+1)
			else
				if type(v) == "string" then
					v = '\"' .. v .. '\"'
				elseif type(v) =="boolean" then
					v = v and "true" or "false"
				elseif type(v) == "userdata" then
					v = "userdata"
				else
					v = tostring(v)
				end
				print(newfmt.."["..k..']'.. " = " .. v .. ',')
			end
		end
		print(right)
	end
end

--
--local t = {{100,2,3}, {100,22,32}, {100,12,13}, {100,213,32}}
--local tt = table.getRandom(t, 3, function (a) 
									--return a[2] 
								--end)
--print("84 =", engine.json.encode(t), engine.json.encode(tt))
table.getRandom = function(list, u, fun)
	local location = {}
	local a = {}
	local total = 0
	local rate = {}
	local n = table.getn(list)
	for i, v in ipairs(list) do
		a[i] = (fun and fun(v) or 1)
		location[i] = i
		total = total + a[i]
		rate[i] = total
	end
	local reduce
	local result = {}
	for i = 1, u do
		local bingo = engine.random(1, total + 1)
		for j = 1, n do
			if rate[j] >= bingo then
				result[i] = location[j]
				if i == u then
					break
				end
				--调整
				total = total - a[j]
				reduce = a[n] - a[j]
				a[j] = a[n]
				rate[j] = a[j] + (rate[j-1] or 0)
				location[j] = location[n]
				for k =  j+ 1, n do
					rate[k] = rate[k] + reduce;
				end
				
				break
			end
		end
		n = n - 1
	end

	return result
end

function Xprint( ... )
	-- body
	print("\n========> Xprint info <========")
	local args = {...}
	for i,value in pairs(args) do
		if type(value) == "table" then
			table.print_t(value)
		elseif type(value) == "string" then
			print('\"' .. value .. '\"')
		elseif type(value) == "userdata" then
			print ("userdata")
		elseif type(value) == "nil" then
			print("this is empty")
		else
			print(value)
		end	
	end

	print("\n")
end