/********************************************************************
	filename: 	Timer.h
	created:	2013/07/12
	author:		shencyx@126.com
	
	purpose:	时间管理
*********************************************************************/
#pragma once

#include <map>
#include "../Common.h"
#include "SharedPtr.h"

namespace shared
{
	class Timer
	{
	public:
		typedef void(*TimerHandle)(void * p);
		struct Entry
		{
			Entry(){ memset(this, 0, sizeof(*this)); }
			uint32 id;
			TimerHandle handle;
			void * p;
		};
		typedef sharedPtr<Entry> EntryPtr;
		typedef std::multimap<uint64, EntryPtr> TimerMap;

	public:
		Timer();

		// 返回毫秒
		static uint32 getTickCount();
		// ==gettimeofday
		static void getTimeOfDay(struct timeval *tv);
		// 当前时间秒
		static uint32 getTime();
		static uint64 getTimeMs();
		// sleep Ms
		static void sleep(uint32 ms);

		uint32 now(){ return _now; }
		uint64 nowMs(){ return _nowMs; }
		void refreshNow();

		uint32 regiestTimer(uint64 timeMs, TimerHandle _handle, void * p);
		void loop();

	private:
		uint32 _now;
		uint64 _nowMs;
		uint32 _idCount;
		TimerMap _timerMap;
	};
}