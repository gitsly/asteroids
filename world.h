#ifndef __World_Included__
#define __World_Included__

#include "cbase.h"

class CWorld : public CBaseObject
{
	public:
		CWorld();
		~CWorld();

		void Draw();
		void Think(); 
		void OnRemove(); 
};

#endif
