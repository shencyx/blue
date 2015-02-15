
#include "../util/config.h"
#include <math.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/des.h>

#if defined(LINUX)
#include <uuid/uuid.h>
#else
#include <objbase.h>
#endif

#include "../util/util.h"
#include "lua_define.h"
#include "lua_crypto.h"

#define LUASTR_CRYPTO_MD5 "md5"
#define LUASTR_CRYPTO_RANDOM "random"
#define LUASTR_CRYPTO_UUID "uuid"
#define LUASTR_CRYPTO_LOCALTIME "localTime"
#define LUASTR_CRYPTO_LOCALTIMEEX "localTimeEx"
#define LUASTR_CRYPTO_TIME "time"
#define LUASTR_CRYPTO_DESDECRYPT "desDecrypt"

namespace lua
{
	void LuaCrypto::registerToLua(lua_State* L)
	{
		lua_getglobal(L, LUASTR_ENGINE);

		lua_pushcfunction(L, &LuaCrypto::md5);
		lua_setfield(L, -2, LUASTR_CRYPTO_MD5);

		lua_pushcfunction(L, &LuaCrypto::random);
		lua_setfield(L, -2, LUASTR_CRYPTO_RANDOM);

		lua_pushcfunction(L, &LuaCrypto::uuid);
		lua_setfield(L, -2, LUASTR_CRYPTO_UUID);

		lua_pushcfunction(L, &LuaCrypto::localTime);
		lua_setfield(L, -2, LUASTR_CRYPTO_LOCALTIME);

		lua_pushcfunction(L, &LuaCrypto::localTimeEx);
		lua_setfield(L, -2, LUASTR_CRYPTO_LOCALTIMEEX);

		lua_pushcfunction(L, &LuaCrypto::time);
		lua_setfield(L, -2, LUASTR_CRYPTO_TIME);

		lua_pushcfunction(L, &LuaCrypto::desDecrypt);
		lua_setfield(L, -2, LUASTR_CRYPTO_DESDECRYPT);
	}

	int LuaCrypto::md5(lua_State* L)
	{
		const char* ptr;
		size_t len;
		MD5_CTX md;
		unsigned char dgst[16];
		char buf[33];

		MD5_Init(&md);
		for (int i = 1; i <= lua_gettop(L); ++i)
		{
			ptr = lua_tolstring(L, i, &len);
			MD5_Update(&md, ptr, len);
		}

		MD5_Final(dgst, &md);
		for (int i = 0; i < 16; ++i)
			snprintf(buf + i*2, 3, "%02x", dgst[i]);
		lua_pushlstring(L, buf, 32);
		return 1;
	}

	int LuaCrypto::random(lua_State* L)
	{
		unsigned x = 0;
		RAND_pseudo_bytes((unsigned char*)&x, sizeof(x));
		double r = (double)x/((double)UINT_MAX+1);

		double min, max;

		switch (lua_gettop(L))
		{
		case 1:
			min = 0;
			max = luaL_optnumber(L, 1, 1);
			break;
		case 2:
			min = luaL_optnumber(L, 1, 0);
			max = luaL_optnumber(L, 2, 1);
			break;
		default:
			min = 0;
			max = 1;
			break;
		}
		if (max <= min)
		{
			lua_pushnumber(L, min);
			return 1;
		}
		r = min + (max - min) * r;

		if (lua_gettop(L) > 0 && min == floor(min) && max == floor(max))
			lua_pushinteger(L, (int)r);
		else
			lua_pushnumber(L, r);
		return 1;
	}

	int LuaCrypto::uuid(lua_State* L)
	{
#if defined(LINUX)
		uuid_t u;
		char buf[40];
		memset(buf, 0, 40);
		uuid_generate(u);
		uuid_unparse(u, buf);
		lua_pushstring(L, buf);
		return 1;
#else
		GUID guid;
		CoCreateGuid(&guid);
		char buf[64];
		memset(buf, 0, 64);
		_snprintf_s(buf, sizeof(buf), "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);
		lua_pushstring(L, buf);
		return 1;
#endif
	}

	int LuaCrypto::localTime(lua_State* L)
	{
		/*struct timeval tv;
		if(gettimeofday(&tv, NULL) == 0)
		{
			double usec = tv.tv_usec/1000000.0;
			lua_pushnumber(L, usec);
			lua_pushnumber(L, tv.tv_sec);
			return 2;
		}
		return 0;*/

		TimeType msec = util::localTime();
		lua_pushnumber(L, msec);
		return 1;
	}
	int LuaCrypto::localTimeEx(lua_State* L)
	{
		TimeType msec = util::localTimeEx();
		lua_pushnumber(L, msec);
		return 1;
	}

	int LuaCrypto::time(lua_State* L)
	{
		int sec = util::time();
		lua_pushinteger(L, sec);
		return 1;
	}

	int LuaCrypto::desDecrypt(lua_State* L)
	{
		const char* inkey;
		const char* incont;
		size_t inkeyLen;
		size_t incontLen;

		// 解密结果
		unsigned char back[64];
		memset(back, 0, sizeof(back));

		if (lua_gettop(L) == 2)
		{
			inkey = lua_tolstring(L, 1, &inkeyLen);
			incont = lua_tolstring(L, 2, &incontLen);

			
			// 秘钥长度固定为8
			if (inkeyLen == 8 && incontLen == 16)
			{
				unsigned char back[17];
				memset(back, 0, sizeof(back));
				
				if (util::desDecrypt(inkey, incont, back) == 0)
				{
					lua_pushstring(L, (char *)back);
					return 1;
				}
				//printf("Decrypted Text: [%s]\n", back);
			}
		}

		//lua_pushnil(L);
		return 0;
	}
}
