/********************************************************************
	filename: 	cmd_struct.h
	created:	2013/12/20
	author:		shencyx@126.com
	
	purpose:	gate与logon 通讯的结构体
*********************************************************************/
#pragma once

namespace protocol {

	enum{
		L2G_LOGIN = 1,
	};

	struct L2GLogin{
		L2GLogin(){ memset(this, 0, sizeof(L2GLogin)); }
		int64 roleId;
		int32 fd;
		int32 type;
	};

}//protocal
