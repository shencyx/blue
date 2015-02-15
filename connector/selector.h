/********************************************************************
	filename: 	selector.h
	created:	2013/10/10
	author:		shencyx@126.com
	
	purpose:	
*********************************************************************/
#pragma once
#include "shared/Common.h"

const static string CLIENT_NAME("client");
const static string LOGON_NAME("logon");

//
static const string & selector1(uint32 hcmd)
{
	return CLIENT_NAME;
}