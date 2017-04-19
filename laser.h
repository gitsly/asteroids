#ifndef __Laser_Included__
#define __Laser_Included__

#include "cbase.h"

class CLaser : public CBaseObject
{
	public:
		CLaser();
		~CLaser();

		float color[3];

		void Draw(); 
		void Think(); 
		void OnRemove(); 
		bool OnBlock(); 
};

#endif
