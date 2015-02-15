package.path = package.path .. ";../?.lua"
require("protocol_router")
require("generate_config")

local ps = string.find(_rpc_name, ".rpc")
assert(ps, "error name:".._rpc_name)
_rpc_File_name = string.sub(_rpc_name, 1, ps - 1)
_out_name = _rpc_File_name .. "_rpc.lua"
if _output_path ~= nil then
	_out_name = _output_path .. "/" .. _out_name
end
print("rpc:", _rpc_name, _rpc_File_name, _service_name, _out_name)

-- class{}: [{return, funName, parameter},]
-- expectation: 1class

local className = nil
local classT = {}
local expectation = 0
local operId = 0

function parse()
	local file = io.open(_rpc_name, "r")
	local line = file:read()

	local lineN = 1
	while line do
		local endPos = string.find(line, "//")
		if endPos then
			line = string.sub(line, 1, endPos - 1)
		end
		--print(line)
		local ret = parseLine(line)
		assert(0 == ret, string.format("something error in line:%d, code:%d", lineN, ret))
		
		if expectation == 2 then
			endClass()
		end
		line = file:read()
		lineN = lineN + 1
	end
	file:close()
	outFile:close()
end

function parseLine(line)
	local tag = {}
	for w in string.gmatch(line, "%a+[%._]?%a+") do
		table.insert(tag, w)
	end
	if expectation == 0 then
		if tag[1] == "class" then
			className = tag[2]
			classT = {}
		end
		if string.find(line, "{") then
			expectation = 1
			if className == nil then
				return 1
			end
			if _G.next(classT) then
				return 2
			end
			operId = _G[_service_name][className]
			if operId == nil or operId == 0 then
				return 3
			end
		end
	elseif expectation == 1 then
		if string.find(line, "}") then
			expectation = 2
			if #tag >= 2 then
				--classT[tag[2]] = {tag[1], tag[3]}
				table.insert(classT, {tag[1], tag[2], tag[3]})
			end
		elseif #tag > 0 then
			if #tag < 2 then
				return 4
			end
			--classT[tag[2]] = {tag[1], tag[3]}
			table.insert(classT, {tag[1], tag[2], tag[3]})
		end
	end
	return 0
end

function endClass()
	-- print("------>", className)
	if outFile == nil then
		outFile = io.open(_out_name, "w")
	end
	writeInclude(outFile)
	writeInit(outFile)
	writeDispatch(outFile)
	--
	for k, v in ipairs(classT) do
		-- print(k, v[1], v[2])
		writeFunc(outFile, v[2], v[3], v[1], k)
	end
	outFile:write('--===========================================================--\n\n')
	
	className = nil
	classT = {}
	expectation = 0
	operId = 0
end

function writeInclude(outFile)
	if _isWriteInclude then
		return
	end
	local L1 = 'local _protobuf = require "protobuf"\n'
	local L2 = 'local _rpcClass = assert(_G["rpcClass"])\n'
	local L3 = 'local _sendPb = assert(__sendPb)\n\n'
	outFile:write(L1)
	outFile:write(L2)
	outFile:write(L3)
	_isWriteInclude = true
end

function writeFuncHead(outFile, funcName, ...)
	local L1 = 'function %s:%s('
	local L2 = ')\n'
	L1 = string.format(L1, className, funcName)
	for i = 1, arg.n do
		L1 = L1 .. arg[i]
		if i ~= arg.n then
			L1 = L1 .. ', '
		end
	end
	L1 = L1 .. L2
	outFile:write(L1)
end

function writeFuncEnd(outFile)
	local L1 = 'end\n\n'
	outFile:write(L1)
end

function writeInit(outFile)
	local L0 = '%s = oo.class(nil, "%s")\n\n'
	L0 = string.format(L0, className, className)
	outFile:write(L0)
	
	writeFuncHead(outFile, "__init")
	assert(operId > 0)
	local L1 = '\tself.operId = %d\n'
	L1 = string.format(L1, operId)
	local L2 = '\tself.operIdFix = self.operId * 65536\n'
	local L3 = '\t_rpcClass[self.operId] = self\n'
	local L4 = '\t_protobuf.register_file("%s/generate/%s.pb")\n'
	L4 = string.format(L4, _service_name, _rpc_File_name)
	local L5 = '\tself.funcList = {}\n'
	
	outFile:write(L1)
	outFile:write(L2)
	outFile:write(L3)
	outFile:write(L4)
	outFile:write(L5)
	
	local L6 = '\tself.funcList[%d] = {"%s", "%s", nil}\n'
	local L7 = '\tself.funcList[%d] = {"%s", "%s", "%s"}\n'
	local L8 = '\tself.funcList[%d] = {"%s", nil, nil}\n'
	local L9 = '\tself.funcList[%d] = {"%s", nil, "%s"}\n'
	local str = nil
	for i = 1, #classT do
		local entry = classT[i]
		if #entry > 2 and entry[1] == "void" then
			str = string.format(L9, i, entry[2], entry[3])
		elseif #entry > 2 and entry[1] ~= "void" then
			str = string.format(L7, i, entry[2], entry[2] .. "Send", entry[3])
		elseif entry[1] == "void" then
			str = string.format(L8, i, entry[2])
		else
			str = string.format(L6, i, entry[2], entry[2] .. "Send")
		end
		outFile:write(str)
	end
	
	writeFuncEnd(outFile)
end

function writeDispatch(outFile)
	writeFuncHead(outFile, "_dispatch", "roleId", "funcId", "buffer", "length")
	--local L0 = '\tfunction account_oper:_dispatch(roleId, funcId, buffer, length)
	local L1 = '\tlocal funcName = self.funcList[funcId]\n'
	local L2 = '\tlocal msg, errorStr = nil, nil\n'
	local L3 = '\tif funcName[3] then\n'
	local L4 = '\t\tmsg, errorStr = _protobuf.decode(funcName[3], buffer, length)\n'
	local L5 = '\tend\n'
	local L6 = '\tif errorStr then\n'
	local L7 = '\t\tprint("error %s:", errorStr, roleId, funcId, length)\n'
	L7 = string.format(L7, className)
	local L8 = '\telse\n'
	local L9 = '\t\tlocal backMsg = self[funcName[1]](self, roleId, msg)\n'
	local L10 = '\t\tif funcName[2] and backMsg then\n'
	local L11 = '\t\t\tself[funcName[2]](self, roleId, backMsg)\n'
	local L12 = '\t\tend\n'
	local L13 = '\tend\n'
	
	outFile:write(L1)
	outFile:write(L2)
	outFile:write(L3)
	outFile:write(L4)
	outFile:write(L5)
	outFile:write(L6)
	outFile:write(L7)
	outFile:write(L8)
	outFile:write(L9)
	outFile:write(L10)
	outFile:write(L11)
	outFile:write(L12)
	outFile:write(L13)
	writeFuncEnd(outFile)
end

function writeFunc(outFile, funcName, parameter, backMsg, operId)
	outFile:write('--------------------------------------\n')
	if parameter == nil or parameter == "void" then
		writeFuncHead(outFile, funcName, "roleId")
	else
		writeFuncHead(outFile, funcName, "roleId", "msg")
	end
	local L1 = '\terror("%s:%s is not implementation!")\n'
	L1 = string.format(L1, className, funcName)
	outFile:write(L1)
	writeFuncEnd(outFile)
	
	--
	if backMsg and backMsg ~= "void" then
		
		writeFuncHead(outFile, funcName .. "Send", "roleId", "msg")
		local L2 = '\tlocal buf = _protobuf.encode("%s", msg)\n'
		L2 = string.format(L2, backMsg)
		local L3 = '\t_sendPb(buf, string.len(buf), self.operIdFix + %d, roleId)\n'
		L3 = string.format(L3, operId)
		outFile:write(L2)
		outFile:write(L3)
		writeFuncEnd(outFile)
	end
end

function main()
	
	parse()
end

main()