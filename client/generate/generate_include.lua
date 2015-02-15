local _protobuf = require "protobuf"
_protobuf.register_file("generate/pb.pb")

__sendPb = function(pointer, length, handleCmd, charId)
	my:sendIns("connector", INS_TYPE.INS_CLIENT_PB, handleCmd, pointer, length, charId)
end

require "generate.account_c_rpc"
require "generate.scene_c_rpc"
require "generate.world_c_rpc"