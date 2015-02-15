--[[


]]

INS_TYPE =
{
	INS_UNKNOW = 0,
	INS_TIMER = 1,	--定时器消息
	INS_LOG = 2,	--日志服务
	INS_MONGO = 3,	--mongo数据库
	INS_MONGO_JSON = 4,	--mongo数据库JSON 格式
	INS_MONGO_BSON = 5,	--mongo数据库BSON 格式
	INS_SERVER_JSON = 6,	--服务器之间通讯
	INS_SERVER_SERI = 7,	--服务器之间通讯
	INS_CLIENT_JSON = 8,	--客户端通讯
	INS_CLIENT_PB = 9,		--客户端通讯
	INS_SERVER_CMD = 10,	--自定义命令
	INS_CLIENT_CMD = 11,
	INS_SERVER_BSON = 12,

	-- 1000以内保留
	INS_USER = 1001,
}