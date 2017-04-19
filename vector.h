#ifndef __Vector3_Included__
#define __Vector3_Included__

class Vector3
{
public:
	Vector3() : x(0.0), y(0.0), z(0.0) {}		 
	Vector3(float, float, float);  

	void operator +=(const Vector3 &);
	Vector3 operator +(const Vector3 &) const;
	void operator -=(const Vector3&);
	Vector3 operator -(const Vector3&) const;
	Vector3 operator *=(const float &);
	Vector3 operator /=(const float&);
	void operator *=(float *); 
	void operator =(const float &);
	Vector3 operator *(float) const;
	float operator *(const Vector3 &) const;
	Vector3 operator -();

	void Vector3::array(float *);

	
	float length() const;
	Vector3 normalize (); 
	void inverse();

	float x,y,z;

};
Vector3 operator *(float, const Vector3 &);
Vector3 crossProduct(const Vector3 &,const Vector3 &);

#endif