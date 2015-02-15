#include <new>
#include <errno.h>
#include "lua_define.h"
#include "lua_iconv.h"

#define LUASTR_ICONV "iconv"
#define LUASTR_ICONV_GTOU "gToU"
#define LUASTR_ICONV_UTOG "uToG"

namespace lua
{
	const char LuaIconv::className[] = LUASTR_ICONV;

#if defined	WINDOWS
	int UTF8ToGBK(const char* sour, std::string & str) 
	{ 
		if(sour == NULL)   
			return -1; 
		int nRetLen = 0; 
		//获取转换到Unicode编码后所需要的字符空间长度 
		nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)sour, -1, NULL, NULL);         
		WCHAR *lpUnicodeStr = new WCHAR[nRetLen + 1];    

		//为Unicode字符串空间 
		//转换到Unicode编码 
		nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)sour, -1, lpUnicodeStr, nRetLen); 
		if(!nRetLen) 
		{ 
			delete []lpUnicodeStr; 
			return -1; 
		} 

		//获取转换到GBK编码后所需要的字符空间长度 
		nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);         

		CHAR *lpGBKStr = new CHAR[nRetLen + 1]; 
		nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, (char *)lpGBKStr, nRetLen, NULL, NULL);       
		//转换到GBK编码 
		if(!nRetLen) 
		{ 
			delete []lpUnicodeStr; 
			delete []lpGBKStr; 
			return -2; 
		} 

		str = lpGBKStr; 
		delete []lpUnicodeStr; 
		delete []lpGBKStr; 
		return 0; 
	}

	int GBKToUTF8(const char* sour, std::string & str) 
	{ 
		if(sour == NULL)   
			return -1; 
		int nRetLen = 0; 
		//获取转换到Unicode编码后所需要的字符空间长度 
		nRetLen = ::MultiByteToWideChar(CP_ACP, 0, (char *)sour, -1, NULL, NULL);         
		WCHAR *lpUnicodeStr = new WCHAR[nRetLen + 1];    

		//为Unicode字符串空间 
		//转换到Unicode编码 
		nRetLen = ::MultiByteToWideChar(CP_ACP, 0, (char *)sour, -1, lpUnicodeStr, nRetLen); 
		if(!nRetLen) 
		{ 
			delete []lpUnicodeStr; 
			return -1; 
		} 

		//获取转换到GBK编码后所需要的字符空间长度 
		nRetLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);         

		CHAR *lpGBKStr = new CHAR[nRetLen + 1]; 
		nRetLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, -1, (char *)lpGBKStr, nRetLen, NULL, NULL);       
		//转换到GBK编码 
		if(!nRetLen) 
		{ 
			delete []lpUnicodeStr; 
			delete []lpGBKStr; 
			return -2; 
		} 

		str = lpGBKStr; 
		delete []lpUnicodeStr; 
		delete []lpGBKStr; 
		return 0; 
	}

#endif


	LuaIconv::LuaIconv(lua_State* L)
	{
		const char* from = luaL_checkstring(L, 2);
		const char* to = luaL_checkstring(L, 3);

#if defined(LINUX)
		_ic = (iconv_t)-1;
		_ic = iconv_open(to, from);
#else
		_from = from;
#endif
	}
	LuaIconv::~LuaIconv()
	{
#if defined(LINUX)
		if (_ic != (iconv_t)-1)
			iconv_close(_ic);
#endif
	}

#define OUTBUF_SIZE 32
	int LuaIconv::conv(lua_State* L)
	{
#if defined(LINUX)
		if (_ic == (iconv_t)-1) 
			return 0;

		size_t inlen, outlen;
		const char* inbuf = luaL_checklstring(L, 2, &inlen);
		char buf[OUTBUF_SIZE];
		char *outbuf;

		iconv(_ic, NULL, NULL, NULL, NULL);
		luaL_Buffer ret;
		luaL_buffinit(L, &ret);
		while(inlen > 0)
		{
			outbuf = buf; outlen = OUTBUF_SIZE;

			if (iconv(_ic, (char**)&inbuf, &inlen, &outbuf, &outlen) == (size_t)-1)
			{
				if (errno != E2BIG) return 0;
			}
			luaL_addlstring(&ret, buf, OUTBUF_SIZE - outlen);
		}
		luaL_pushresult(&ret);
		return 1;
#else
		if(_from == "utf-8" || _from == "UTF-8")   //utf8 to gbk
		{
			size_t inlen;
			const char* inbuf = luaL_checklstring(L, 2, &inlen);

			std::string outbuf;
			if(UTF8ToGBK(inbuf, outbuf) != 0)
			{
				if (errno != E2BIG) return 0;
			}
			lua_pushstring(L, outbuf.c_str());
			return 1;
		}
		else if(_from == "gbk" || _from == "GBK")	//gbk to utf8
		{
			size_t inlen;
			const char* inbuf = luaL_checklstring(L, 2, &inlen);

			std::string outbuf;
			if(GBKToUTF8(inbuf, outbuf) != 0)
			{
				if (errno != E2BIG) return 0;
			}
			lua_pushstring(L, outbuf.c_str());
			return 1;
		}

		return 0;
#endif
	}

	void LuaIconv::registerToLua(lua_State* L)
	{
		lua::LuaClass<LuaIconv> lc(L);
		lc.defMeta<&LuaIconv::conv>("__call");

		int sp = lua_gettop(L);
		lua_getglobal(L, LUASTR_ENGINE);
		//engine.uToG
		lua_pushstring(L, LUASTR_ICONV_UTOG);
		char cc[256]={0};
		snprintf(cc, sizeof(cc)-1, "return %s(\"utf-8\", \"gbk\")", className);
		if(luaL_dostring(L, cc) != 0)
		{
			luaL_error(L, "%s load oo.class is error!", className);
		}
		lua_settable(L, -3);

		//engine.gToU
		lua_pushstring(L, LUASTR_ICONV_GTOU);
		snprintf(cc, sizeof(cc)-1, "return %s(\"gbk\", \"utf-8\")", className);
		if(luaL_dostring(L, cc) != 0)
		{
			luaL_error(L, "%s load oo.class is error!", className);
		}
		lua_settable(L, -3);
		lua_settop(L, sp);
	}
}
