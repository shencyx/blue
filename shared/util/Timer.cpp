#include "Timer.h"

#ifdef WIN32

#else
#include <sys/timeb.h>
#include <sys/time.h>
#endif


namespace shared
{
	Timer::Timer():
		_now(0),
		_nowMs(0),
		_idCount(0)
	{
		refreshNow();
	}

	uint32 Timer::getTickCount()
	{
#ifdef WIN32
		return GetTickCount();
#else
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
	}

	void Timer::getTimeOfDay(struct timeval *tv)
	{
#ifdef WIN32
		/*
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;
		GetLocalTime(&wtm);
		tm.tm_year = wtm.wYear - 1900;
		tm.tm_mon = wtm.wMonth - 1;
		tm.tm_mday = wtm.wDay;
		tm.tm_hour = wtm.wHour;
		tm.tm_min = wtm.wMinute;
		tm.tm_sec = wtm.wSecond;
		tm.tm_isdst = -1;
		clock = mktime(&tm);
		tv->tv_sec = (long)clock;
		tv->tv_usec = wtm.wMilliseconds * 1000;
		*/
		SYSTEMTIME wtm;
		GetLocalTime(&wtm);
		tv->tv_sec = (long)::time(0);
		tv->tv_usec = wtm.wMilliseconds * 1000;
#else
		gettimeofday(&tv, NULL);
#endif
	}

	void Timer::sleep(uint32 ms)
	{
#ifdef WIN32
		::Sleep(ms);
#else
		::usleep(ms * 1000);
#endif
	}

	uint32 Timer::getTime()
	{
		struct timeval tv;
		getTimeOfDay(&tv);
		return tv.tv_sec;
	}

	uint64 Timer::getTimeMs()
	{
		struct timeval tv;
		getTimeOfDay(&tv);
		return (((uint64)tv.tv_sec) * 1000 + tv.tv_usec / 1000);
	}

	void Timer::refreshNow()
	{
		struct timeval tv;
		getTimeOfDay(&tv);
		_now = tv.tv_sec;
		_nowMs = (((uint64)tv.tv_sec) * 1000 + tv.tv_usec / 1000);
	}

	uint32 Timer::regiestTimer(uint64 timeMs, TimerHandle _handle, void * p)
	{
		EntryPtr pEntry(new Entry());
		pEntry->id = ++_idCount;
		pEntry->handle = _handle;
		pEntry->p = p;
		_timerMap.insert(std::make_pair(timeMs, pEntry));
		return pEntry->id;
	}

	void Timer::loop()
	{
		refreshNow();
		while(!_timerMap.empty() && _timerMap.begin()->first <= _nowMs)
		{
			EntryPtr pEntry = _timerMap.begin()->second;
			_timerMap.erase(_timerMap.begin());

			pEntry->handle(pEntry->p);
		}
	}
}