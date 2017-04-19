#ifndef __Particle_Included__
#define __Particle_Included__

#include "cbase.h"

class CParticle : public CBaseObject
{
	public:
		CParticle();
		~CParticle();

		float color[3];
		float friction;
		Vector3 move;
		long life;

		void Draw(); 
		void Think(); 
		bool OnBlock(); 
		virtual void OnAge() {};
};



class CFadeParticle : public CParticle
{
	public:
		CFadeParticle();

		float colorChange[3];
		float fadeRate, scaleRate;
		void OnAge();
};

// Aging functions must be global.


#endif
