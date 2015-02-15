

table.size = function(o) 
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
		return
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

table.isEmpty = function(t)
	return next(t) == nil
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

-- 树型打印一个 table,不用担心循环引用
table.print_r = function(root)
--function print_r(root)
	local cache = { [root] = "." }
	print("{")
	local function _dump(t,space,name)
		local temp = {}
		for k,v in pairs(t) do
			local key = tostring(k)
			if type(k) == "string" then
				key ='\"' .. tostring(k) .. '\"'
			end
			
			if cache[v] then
				table.insert(temp,space .. "["..key.."]" .." = " .. " {" .. cache[v].."},")
			elseif type(v) == "table" then
				local new_key = name .. "." .. tostring(k)
				cache[v] = new_key
				
				local tableStr = _dump(v,space .. (next(t,k) and "|" or "") .. string.rep(" ",#key<4 and 4 or #key),new_key)
				if tableStr then		-- 非空table
					table.insert(temp,space .. "["..key.."]" .." = " .. "{")
					table.insert(temp, tableStr)
					table.insert(temp,space .."},")
				else 						-- 空table
					table.insert(temp,space .. "["..key.."]" .." = " .. "{ },")
				end
				
				--table.insert(temp, _dump(v,space .. (next(t,k) and "|" or " " ).. string.rep(" ",#key),new_key))
			else
				if type(v) == "string" then
					v = '\"' .. v .. '\"'
				else
					v = tostring(v)
				end
				table.insert(temp,space .. "["..key.."]" .. " = " .. v ..",")
				--tinsert(temp,"+" .. key .. " [" .. tostring(v).."]")
			end
		end
		return #temp>0 and table.concat(temp,"\n") or nil
	end
	local allTableString = _dump(root, "    ","")
	print(allTableString or "")
	print("}")
end


function Xprint( ... )
	-- body
	--print("\n========> Xprint info <========")
	if DEBUG then
		print("\n")

		--local start = os.clock()
		for i=1,arg.n do
			local value = arg[i]
			if type(value) == "table" then
				table.print_r(value)
			elseif type(value) == "string" then
				print('\"' .. value .. '\"')
			else
				print(tostring(value))
			end	
		end

		print("\n")
	else
		print("###==========>>>This is Release Version!!!!!")
	end
	
	--print(string.format("-------------> Xprint_Used_Time : %d ms <-------------",os.clock() - start),"\n")
	-- local args = {...}
	-- for i,value in pairs(args) do
	-- 	if type(value) == "table" then
	-- 		table.print_r(value)
	-- 	elseif type(value) == "string" then
	-- 		print('\"' .. value .. '\"')
	-- 	else
	-- 		print(tostring(value))
	-- 	end	
	-- end
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
	if u > n then
		u = n
	end
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
