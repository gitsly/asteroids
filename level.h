#ifndef __Level_Included__
#define __Level_Included__

class Vector3;
template <class T> class CLink;

class CLevelObjects
{
	public:
		float x, y; // where to spawn.
		long time; // when to spawn. 0 at level start -x random promille.
		char name[512]; // of object.

};

class CLevel
{
	public:
		char world[512];
		char levelname[512];
		float asteroidSpeed;

		CLink<CLevelObjects> *object;
};

#endif
