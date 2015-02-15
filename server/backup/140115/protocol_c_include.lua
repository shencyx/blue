local _protobuf = require "protobuf"
_protobuf.register_file(PROTOCOL_PATH.."/pb.pb")


__sendPb = function(pointer, length, handleCmd, charId)
	my:sendIns("connector", INS_TYPE.INS_CLIENT_PB, handleCmd, pointer, length, charId)
end

require "logon.protocol_c"