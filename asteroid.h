#ifndef __Asteroid_Included__
#define __Asteroid_Included__

#include "cbase.h"

class CAsteroid : public CBaseObject
{
	public:
		CAsteroid();
		~CAsteroid();

		float mod[3];

		void Draw(); 
		void Think(); 
		void OnRemove(); 
		bool OnBlock(); 
};

#endif
