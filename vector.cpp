/* Filename:	vector.cpp
 * Author:		 Martin Collberg
 * Email:		  comayog@swipnet.se
 *
 * Vector code, lots of operators.
 *
 * /


/*
#include <windows.h>
#include <GL/gl.h>
*/

#include <math.h>
#include "vector.h"

//------------------
// Vector code   
//------------------

Vector3::Vector3(float x1, float y1, float z1)
{
	x = x1;
	y = y1;
	z = z1;
}

void Vector3::operator +=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}


Vector3 Vector3::operator +(const Vector3& v) const
{
	Vector3 w(*this);
	w += v;
	return w;
}


void Vector3::operator -=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}


Vector3 Vector3::operator -(const Vector3& v) const
{
	Vector3 w( * this);
	w -= v;
	return w;
}

void Vector3::operator = (const float &v)
{
	x = v;
	y = v;
	z = v;
}


void Vector3::operator *=(float *f)
{
	x *= f[0];
	y *= f[1];
	z *= f[2];
}

Vector3 Vector3::operator *=(const float &mul)
{
	x *= mul;
	y *= mul;
	z *= mul;
	return *this;
}


Vector3 Vector3::operator *(float f) const
{
	Vector3 w(*this);
	w *= f;
	return w;
}


Vector3 operator *(float f,const Vector3 &v)
{
	Vector3 w;
	w = v * f;
	return w;
}


float Vector3::operator *(const Vector3 &v) const
{
	return x * v.x + y * v.y + z * v.z;
}


Vector3 crossProduct(const Vector3 &u,const Vector3 &v)
{
	Vector3 w;
	w.x = u.y * v.z - u.z * v.y;
	w.y = u.z * v.x - u.x * v.z;
	w.z = u.x * v.y - u.y * v.x;
	return w;
}

Vector3 Vector3::operator /=(const float &div)
{
	x /= div;
	y /= div;
	z /= div;
	return *this;
}

Vector3 Vector3::operator -()
{
	Vector3 v;
	v.x = -x;
	v.y = -y;
	v.z = -z;
	return v;
}


void Vector3::array( float *v)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}


float Vector3::length() const
{
	return (float)sqrt(x * x + y * y + z * z);
}

void Vector3::inverse()
{
	x = -x;
	y = -y;
	z = -z;
}

Vector3 Vector3::normalize ()
{
	float l = length();
	if (l != 0.0)
	{
		x /= l;
		y /= l;
		z /= l;
	}
	return *this;	
}

