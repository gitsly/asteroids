#include <dinput.h>
#include "mouse.h"
#include "util.h"

#define INITGUID

CMouse::CMouse(HWND windowHandle, IDirectInput *directInput, HINSTANCE handleInstance)
{
	HRESULT hr;

	directMouse = NULL;
    
	// Register with the DirectInput subsystem and get a pointer
	// to a IDirectInput interface we can use.
	hr = DirectInputCreate( handleInstance, DIRECTINPUT_VERSION, &directInput, NULL );
	if ( FAILED(hr) ) 
	  end("error initializing Direct Input");

	// Obtain an interface to the system mouse device.
	hr = directInput->CreateDevice( GUID_SysMouse, &directMouse, NULL );
	if ( FAILED(hr) ) 
	  end("couldn't create handle for mouse device");

	// Set the data format to "mouse format" - a predefined data format 
	//
	// A data format specifies which controls on a device we
	// are interested in, and how they should be reported.
	//
	// This tells DirectInput that we will be passing a
	// DIMOUSESTATE structure to IDirectInputDevice::GetDeviceState.
	hr = directMouse->SetDataFormat( &c_dfDIMouse );
	if ( FAILED(hr) ) 
	  end("couldn't set data format return type\n for mouse object");

	// Set the cooperativity level to let DirectInput know how
	// this device should interact with the system and with other
	// DirectInput applications.
	hr = directMouse->SetCooperativeLevel( windowHandle, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if ( FAILED(hr) ) 
	  end("failed to set cooperative level for mouse device");

	directMouse->Acquire();

}


//-----------------------------------------------------------------------------
// Function: UpdateInputState
//
// Description: 
//      Get the input device's state and display it.
//
//-----------------------------------------------------------------------------
void CMouse::ReadMouse()
{
	if(!directMouse)
		return;
	
	DIMOUSESTATE dims;      // DirectInput mouse state structure
	HRESULT hr;

	hr = DIERR_INPUTLOST;

	// if input is lost then acquire and keep trying 
	while ( hr == DIERR_INPUTLOST ) 
	{
		// get the input's device state, and put the state in dims
		hr = directMouse->GetDeviceState( sizeof(DIMOUSESTATE), &dims );

		if ( hr == DIERR_INPUTLOST )
		{
			 // DirectInput is telling us that the input stream has
			 // been interrupted.  We aren't tracking any state
			 // between polls, so we don't have any special reset
			 // that needs to be done.  We just re-acquire and
			 // try again.
			 hr = directMouse->Acquire();
			 if ( FAILED(hr) )  
				  return;
		}
	}

	if ( FAILED(hr) )  
		return;

	button[0] = dims.rgbButtons[0] & 0x80;	
	button[1] = dims.rgbButtons[1] & 0x80;	
	button[2] = dims.rgbButtons[2] & 0x80;	
	button[3] = dims.rgbButtons[3] & 0x80;	
	
	mx = dims.lX;
	my = dims.lY;
}




CMouse::~CMouse()
{
    // Unacquire and release any DirectInputDevice objects.
    if (directMouse) 
    {
        // Unacquire the device one last time just in case 
        // the app tried to exit while the device is still acquired.
        directMouse->Unacquire();
        directMouse->Release();
        directMouse = NULL;
    }
}

