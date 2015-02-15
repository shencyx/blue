/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2008-2012 <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef RWLOCK_H
#define RWLOCK_H

#include "../Common.h"
#include "Mutex.h"

#ifdef WIN32
class RWLock
{
	public:
		ARCEMU_INLINE void AcquireReadLock()
		{
			_lock.Acquire();
		}

		ARCEMU_INLINE void ReleaseReadLock()
		{
			_lock.Release();
		}

		ARCEMU_INLINE void AcquireWriteLock()
		{
			_lock.Acquire();
		}

		ARCEMU_INLINE void ReleaseWriteLock()
		{
			_lock.Release();
		}

		//
		ARCEMU_INLINE void rLock()
		{
			_lock.Acquire();
		}

		ARCEMU_INLINE void rUnLock()
		{
			_lock.Release();
		}

		ARCEMU_INLINE void wLock()
		{
			_lock.Acquire();
		}

		ARCEMU_INLINE void wUnLock()
		{
			_lock.Release();
		}

	private:
		Mutex _lock;
};

#else
	//
class RWLock
{
public:
	RWLock():_write(0),_read(0){}

	ARCEMU_INLINE void AcquireReadLock()
	{
		for (;;) {
			while(_write) {
				__sync_synchronize();
			}
			__sync_add_and_fetch(&_read,1);
			if (_write) {
				__sync_sub_and_fetch(&_read,1);
			} else {
				break;
			}
		}
	}

	ARCEMU_INLINE void ReleaseReadLock()
	{
		__sync_sub_and_fetch(&_read,1);
	}

	ARCEMU_INLINE void AcquireWriteLock()
	{
		while (__sync_lock_test_and_set(&_write,1)) {}
		while(_read) {
			__sync_synchronize();
		}
	}

	ARCEMU_INLINE void ReleaseWriteLock()
	{
		__sync_lock_release(&_write);
	}

	//
	ARCEMU_INLINE void rLock()
	{
		AcquireReadLock();
	}

	ARCEMU_INLINE void rUnLock()
	{
		ReleaseReadLock();
	}

	ARCEMU_INLINE void wLock()
	{
		AcquireWriteLock();
	}

	ARCEMU_INLINE void wUnLock()
	{
		ReleaseWriteLock();
	}

private:
	int _write;
	int _read;
};
#endif

#endif
