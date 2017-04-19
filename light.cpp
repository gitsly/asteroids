#include <memory.h> 
#include "light.h"

CLight::CLight()
{
	memset(&col, 0, sizeof(col));
	memset(&pos, 0, sizeof(pos));
}

CLight::~CLight()
{

}

void CLight::SetPosition( float x, float y, float z , float a )		
{
	pos[0] = x; 
	pos[1] = y; 
	pos[2] = z; 
	pos[3] = a; 
}

void CLight::SetColor( float r, float g, float b, float a )		
{
	col[0] = r; 
	col[1] = g; 
	col[2] = b; 
	col[3] = a; 
}

