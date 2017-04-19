#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

#include <dinput.h>

class CMouse
{
public:
	CMouse() {} 
	CMouse(HWND windowHandle, IDirectInput *directInput, HINSTANCE handleInstance);
	~CMouse();
	
	void ReadMouse();

	int mx;
	int my;
	int button[4];

	bool active;

	IDirectInputDevice	*directMouse;
};

#endif
