#ifndef __Edict_Included__
#define __Edict_Included__

#include "vector.h"

class C3dObject;
class CBaseObject;
class Vector3;

class CEdict : public Vector3
{
	public:
		CEdict();
		~CEdict();

		bool deleted; 
		int frame;
		float pitch, roll, angle, scale, trans, radius;
		Vector3 mom; // Momentaries.

		CBaseObject *pev; // Pointer to edict vars (class that controls the edict, very virtual at it's base form).		
		C3dObject *model;		

		void CalcRadius();
		void Remove();
		void BounceOnBorder();
		void LoopIfBeyondBorder();
		CEdict*  CollisionWithin(float dist);
		void CopyPosition( CEdict * );

};

#endif
