require "global_define"
local INS_TYPE = INS_TYPE

module("mongo_oper", package.seeall)

--[[
	
	"cmd", string
	"ns", string
	"update", string
	"query", string
	"fields", string
	"upsert", bool
	"multi", bool
]]

function getCache(ns, query)
	assert(ns)
	local data = {}
	data.cmd = "getCache"
	data.ns = ns
	data.query = query or {}
	--data.fields= fields or {}
	return my:sendIns("mongo", INS_TYPE.INS_MONGO_JSON, "getCache", json.encode(data))
end

function find(ns, query, fields)
	assert(ns)
	local data = {}
	data.cmd = "find"
	data.ns = ns
	data.query = query or {}
	data.fields= fields or {}
	return my:sendIns("mongo", INS_TYPE.INS_MONGO_JSON, "find", json.encode(data))
end

function findOne(ns, query, fields)
	assert(ns)
	local data = {}
	data.cmd = "findOne"
	data.ns = ns
	data.query = query or {}
	data.fields= fields or {}
	return my:sendIns("mongo", INS_TYPE.INS_MONGO_JSON, "findOne", json.encode(data))
end