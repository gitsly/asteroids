#ifndef __LightClass_Included__
#define __LightClass_Included__


class CLight
{
	public:
		CLight();
		~CLight();

		float pos[4];	// x, y, z, wierd.		
		float col[4]; // r, g, b, a.
		
		void SetPosition( float, float, float, float );		
		void SetColor( float, float, float, float );		
};


#endif