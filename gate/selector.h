/********************************************************************
	filename: 	selector.h
	created:	2013/10/10
	author:		shencyx@126.com
	
	purpose:	消息包定向
	classId :
	< 1000 --保留
	1000 - 1999 --logon
	2000 - 2999 --world
	3000 - 3000 --scene

*********************************************************************/
#pragma once
#include "shared/Common.h"

const static string CLIENT_NAME("client");
const static string LOGON_NAME("logon");
const static string SCENE_NAME("scene");
const static string WORLD_NAME("world");
const static string LOG_NAME("log");

//
static const string & selector1(uint32 hcmd)
{
	uint16 classId = (uint16)(hcmd >> 16);
	uint16 funcId =  (uint16)hcmd;
	//printf("selector:%d, %d\n", classId, funcId);
	if (classId < 1000)
	{
		return LOG_NAME;
	} 
	else if (classId < 2000)
	{
		return LOGON_NAME;
	}
	else if (classId < 3000)
	{
		return WORLD_NAME;
	}
	else if (classId < 4000)
	{
		return SCENE_NAME;
	}

	return LOG_NAME;
}