#ifndef __Ship_Included__
#define __Ship_Included__

#include "cbase.h"

class CShip : public CBaseObject
{
	public:
		CShip();
		~CShip();

		bool thrust;
		int frame;
		float speed, maxSpeed, frictionMod;

		void Draw();
		void Think(); 

		void OnFire();
		void OnMoveForward();
		void OnMoveBackward();

		void OnRemove(); 
		bool OnBlock(); 

};

#endif
