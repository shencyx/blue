local _protobuf = require "protobuf"
_protobuf.register_file(PROTOCOL_PATH.."/pb.pb")

assert(_G["rpcClass"] == nil)
_G["rpcClass"] = {}

__sendPb = function(pointer, length, handleCmd, charId)
	my:sendIns("gate", INS_TYPE.INS_CLIENT_PB, handleCmd, pointer, length, charId)
end

require "logon.protocol"