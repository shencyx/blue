#pragma once
//#include "config.h"
 #include <cstdlib>

namespace shared
{
	template < class type > 
	class Singleton
	{
	public:
		Singleton()
		{
			//this->mSingleton = static_cast<type *>(this);
			//_singleton = NULL;
		}

		~Singleton() 
		{
			//this->mSingleton = NULL;
		}

		inline static type & getSingleton() 
		{ 
			/*static type* single = NULL;
			if(single == NULL)
			{
				single = new type();
			}
			return *single; */

			if(_singleton == NULL)
			{
				_singleton = new type();
			}
			return *_singleton;
		}
		inline static type * getSingletonPtr() 
		{ 
			if(_singleton == NULL)
			{
				_singleton = new type();
			}
			return _singleton;
		}
	protected:
		static type* _singleton;
	};

	template < class type >
	type* Singleton<type>::_singleton = NULL;
}
