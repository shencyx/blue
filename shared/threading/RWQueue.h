/********************************************************************
	filename: 	RWQueue.h
	created:	2013/07/27
	author:		shencyx@126.com
	
	purpose:	读写队列, 只能单线程读数据
*********************************************************************/
#pragma once

#include <queue>
#include "Mutex.h"

namespace shared
{

template <class T>
class RWQueue
{
public:
	typedef std::queue<T> queue;

public:
	RWQueue()
	{
		_rq = &_q[0];
		_wq = &_q[1];
	}
	virtual ~RWQueue()
	{
		clear();
	}

	inline size_t rqSize()
	{
		return _rq->size();
	}
	inline void push(const T & m)
	{
		_lock.lock();
		_wq->push(m);
		_lock.unlock();
	}
	//inline T pop()
	//{
	//	if (!_rq->empty())
	//	{
	//		return _rq->pop();
	//	}
	//	swap();
	//	return _rq->pop();
	//}
	inline T frontPop()
	{
		if (!_rq->empty())
		{
			T e = _rq->front();
			_rq->pop();
			return e;
		}
		if (!_wq->empty())
		{
			swap();
			T e = _rq->front();
			_rq->pop();
			return e;
		}
		return T();
	}
	inline void swap()
	{
		queue * t = _rq;
		_lock.lock();
		_rq = _wq;
		_wq = t;
		_lock.unlock();
	}
	void clear()
	{
		_lock.lock();
		//_q[1].clear();
		//_q[2].clear();
		_lock.unlock();
	}

private:
	queue _q[2];
	queue * _rq;
	queue * _wq;
	Mutex _lock;
};

}//shared

