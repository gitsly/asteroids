#ifndef __Engine_H__
#define __Engine_H__

#include <windows.h>
#include <dinput.h>

#define appTitle "Asteroids by M.C (comayog@swipnet.se)"


#ifndef M_PI
	#define M_PI 3.14159265
#endif

template <class T> class CLink;
class CLight;
class CEdict;
class CMouse;
class CTextureBase;
class C3dObject;
class CGame;

class CEngine
{
	public:
		CEngine();
		~CEngine();
		CTextureBase *textures;
		CLink <C3dObject> *model;
		CLink <CLight> *light;
		CLink <CEdict> *edict;

		// Inputs		
		CMouse *mouse;
		bool	key[256];	

		// Raw app data.
		bool	active;
		bool	fullscreen;
		char	title[256];
		float	fov;
		int       xres, yres;
		unsigned int timer;
		float	 desiredFps;

		char baseDir[128];
		void ChangeDir(char *dir);
		void RawUpdate();
		void SetupLights();

		// Game specific data.
		CGame *game;

		C3dObject* GetModelByName(char *name);
};

// Most global vars.
extern CEngine *engine;
extern HWND			hWnd;		
extern HINSTANCE	hInstance;	
extern IDirectInput *directInput;

void KillGLWindow();	
bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag);


#endif