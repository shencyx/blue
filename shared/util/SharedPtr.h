/*
 * sharedPtr.h
 *
 */

#ifndef shared_ptr_H_
#define shared_ptr_H_

//#define SharedPtr_TEST
#ifdef SharedPtr_TEST
#include <iostream>
using namespace std;
#endif

#include "../Common.h"

namespace shared
{

template <typename T>
class SharedPtr
{
public:
	SharedPtr():_p(NULL), _ref(NULL)
	{
	}
//	explicit
	SharedPtr(T * p):_p(p), _ref(new uint32(1))
	{
		ASSERT(_p);
#ifdef SharedPtr_TEST
		cout << endl;
		cout << "shared ptr: structor" << endl;
		cout << "p: " << _p << " ref: " << _ref << endl;
		cout << endl;
#endif
	}

	SharedPtr(const SharedPtr & src):_p(src._p), _ref(src._ref)
	{
		ASSERT(_p);
		ref_count_add(1);
#ifdef SharedPtr_TEST
		cout << endl;
		cout << "shared ptr: copy structor" << endl;
		cout << "p: " << _p << " ref: " << _ref << endl;
		cout << endl;
#endif
	}

	SharedPtr & operator= (const SharedPtr& rhs)
	{
		ASSERT(rhs._p);
		if((_p == rhs._p) && (_ref == rhs._ref))
		{
			return *this;
		}
		if (_p)
			ref_count_add(-1);
		_p = rhs._p;
		_ref = rhs._ref;
		ref_count_add(1);
		return *this;
	}

	T* get()
	{
		return _p;
	}

	T* operator->()
	{
		if(!_p)
		{
			ASSERT(false);
		}
		return _p;
	}

	const T * operator->() const
	{
		if(!_p)
		{
			ASSERT(false);
		}
		return _p;
	}

	T & operator*()
	{
		if(!_p)
		{
			ASSERT(false);
		}
		return *_p;
	}
	const T & operator*() const
	{
		if(!_p)
		{
			ASSERT(false);
		}
		return *_p;
	}

	~SharedPtr()
	{
#ifdef SharedPtr_TEST
		cout << endl;
		cout << "p: " << _p << " ref: " << _ref << endl;
		cout << "shared ptr: destructor" << endl;
		cout << endl;
#endif
		ref_count_add(-1);
	}

private:
	inline void ref_count_add(uint32 add_value)
	{
		//ASSERT(_ref);
		if(_ref)
		{
#ifdef WIN32
			if (add_value == 1)
				InterlockedIncrement(reinterpret_cast< volatile uint32 * >(_ref));
			else if (add_value == -1)
				InterlockedDecrement(reinterpret_cast< volatile uint32 * >(_ref));
#else
			__sync_add_and_fetch(_ref, add_value)
#endif
			if(*_ref == 0)
			{
				ASSERT(_p);
				if(_p)
				{
#ifdef SharedPtr_TEST
		cout << "shared ptr: delete p: " << _p << endl;
#endif
					delete _p;
					_p = NULL;
				}

#ifdef SharedPtr_TEST
		cout << "shared ptr: delete ref: " << _ref << endl;
#endif
				delete _ref;
				_ref = NULL;
			}
		}
	}

	T * _p;
	uint32 * _ref;
};

#define sharedPtr shared::SharedPtr
//typedef shared::SharedPtr<> sharedPtr<>;

} //namespace shared
#endif /* shared_ptr_H_ */
