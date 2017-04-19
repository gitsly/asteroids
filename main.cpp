#include <windows.h>
#include <GL/glut.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "engine.h"
#include "game.h"
#include "util.h"
#include "resource.h"

/****************
      MAKEME's
*****************
- do the game.

- do fullscreen flips stable. (add resolution change).

*/

HDC					 hDC=NULL;		// Private GDI Device Context
HGLRC			   hRC=NULL;		// Permanent Rendering Context
HWND		      hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	  hInstance;		// Holds The Instance Of The Application
IDirectInput	 *directInput;

CEngine *engine;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

void ReSizeGLScene(int width, int height)	
{
	if (!height)	
		height=1;		

	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				

	if(engine)
		gluPerspective(engine->fov, (float)width/(float)height,  0.1, 400.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int InitGL()	
{
	directInput = NULL;

	glEnable(GL_LIGHTING );		
	glEnable(GL_TEXTURE_2D );							
	glShadeModel(GL_SMOOTH);							
	glClearColor(0.2, 0.3, 7, 0.0);				
	glClearDepth(1.0);									
	glEnable(GL_DEPTH_TEST);							
	glDepthFunc(GL_LEQUAL);								
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	

	glMatrixMode(GL_MODELVIEW);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);  
	glDisable(GL_BLEND);  

	return true;
}

void KillGLWindow()					
{

	if (directInput) 
	{
		directInput->Release();
		directInput = NULL;
	}

	ChangeDisplaySettings(NULL,0); // switch back to the desktop
	ShowCursor(true);						

	if (hRC)										
	{
		if (!wglMakeCurrent(NULL,NULL))			
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);

		if (!wglDeleteContext(hRC))					
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hRC=NULL;								
	}

	if (hDC && !ReleaseDC(hWnd,hDC))
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;								
	}

	if (hWnd && !DestroyWindow(hWnd))
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;									
	}

	if (!UnregisterClass("OpenGL",hInstance))		
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;	
	}

	if(engine)
		KillTimer(hWnd, engine->timer);
}

bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;	
	WNDCLASS	wc;				
	DWORD		dwExStyle;		
	DWORD		dwStyle;			
	RECT		WindowRect;			
	WindowRect.left=(long)0;		
	WindowRect.right=(long)width;	
	WindowRect.top=(long)0;		
	WindowRect.bottom=(long)height;	

	hInstance			= GetModuleHandle(NULL);			
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	
	wc.lpfnWndProc		= (WNDPROC) WndProc;	
	wc.cbClsExtra		= 0;			
	wc.cbWndExtra		= 0;			
	wc.hInstance		= hInstance;	
	wc.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);	
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);	
	wc.hbrBackground	= NULL;								
	wc.lpszMenuName		= NULL;							
	wc.lpszClassName	= "OpenGL";					

	if(!wc.hIcon)
		end("couldn't load icon for app\n");

	if (!RegisterClass(&wc))		
		end("Failed To Register The Window Class.");
	
	if (fullscreenflag)				
	{
		DEVMODE dmScreenSettings;	
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);	
		dmScreenSettings.dmPelsWidth	= width;				
		dmScreenSettings.dmPelsHeight	= height;				
		dmScreenSettings.dmBitsPerPel	= bits;					
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				fullscreenflag = false;
			else
				end("Program Will Now Close.");
		}
	}

	if (fullscreenflag)		
	{
		dwExStyle=WS_EX_APPWINDOW;		
		dwStyle=WS_POPUP;							
		ShowCursor(true);								
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	
		dwStyle=WS_OVERLAPPEDWINDOW;							
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);		

	if (!(hWnd=CreateWindowEx(	dwExStyle, "OpenGL", title,	dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top,	NULL, NULL,	hInstance, NULL)))							
		end("Window Creation Error.");

	static	PIXELFORMATDESCRIPTOR pfd =		
	{
		sizeof(PIXELFORMATDESCRIPTOR),			
		1,										
		PFD_DRAW_TO_WINDOW |			
		PFD_SUPPORT_OPENGL |				
		PFD_DOUBLEBUFFER,					
		PFD_TYPE_RGBA,							
		bits,	
		0, 0, 0, 0, 0, 0,	
		0,						
		0,						
		0,						
		0, 0, 0, 0,			
		16,					
		0,					
		0,					
		PFD_MAIN_PLANE,			
		0,									
		0, 0, 0							
	};
	
	if (!(hDC = GetDC(hWnd)))
		end("Can't Create A GL Device Context.");

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	
		end("Can't Find A Suitable PixelFormat.");

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))	
		end("Can't Set The PixelFormat.");
		
	if (!(hRC = wglCreateContext(hDC)))	
		end("Can't Create A GL Rendering Context.");

	if(!wglMakeCurrent(hDC, hRC))		
		end("Can't Activate The GL Rendering Context.");

	ShowWindow(hWnd,SW_SHOW);	
	SetForegroundWindow(hWnd);		
	SetFocus(hWnd);						

	ReSizeGLScene(width, height);	

	if (!InitGL())		
		end("Initialization Failed.");
	
	return true;	
}

LRESULT CALLBACK WndProc(	HWND	hWnd,	UINT	msg,	WPARAM	wParam, LPARAM	lParam)
{
	switch (msg)
	{

		case WM_TIMER:		
			engine->game->ticks ++;
			break;	

		case WM_ACTIVATE:		
			if(engine)
				engine->active =  (!HIWORD(wParam)) ? true : false;					
			return 0;				

		case WM_SYSCOMMAND:		
			switch (wParam)						
			{
				case SC_SCREENSAVE:				
				case SC_MONITORPOWER:		
					return 0;			
			}
			break;						

		case WM_CLOSE:				
			PostQuitMessage(0);		
			return 0;					
		case WM_KEYDOWN:			
			engine->key[wParam] = true;			
			return 0;								
		case WM_KEYUP:	
			engine->key[wParam] = false;
			return 0;							
		case WM_SIZE:
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam)); 
			return 0;						
	}

	return DefWindowProc(hWnd, msg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance, HINSTANCE	hPrevInstance, LPSTR		lpCmdLine, int	nCmdShow)
{
	MSG		msg;		
	int i;
	bool	done = false;	

	if (!CreateGLWindow(appTitle, 640, 480, 16, false))
		return 0;		

	engine = new CEngine;
	engine->game->Init();

	engine->timer = SetTimer(hWnd, engine->timer, 10, NULL);

	ReSizeGLScene(engine->xres, engine->yres);

	while(!done)
	{

		engine->game->ticks = 1;  // Remove this line to see the louse timer in action....
		for(i = 0; i < engine->game->ticks; i ++) 
			engine->game->Think();
		engine->game->ticks = 0;
		
		if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
				done=true;					
			else								
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else		
		{
			engine->RawUpdate();
			if (engine->key[VK_ESCAPE])
				done = true;	
			else					
				SwapBuffers(hDC);

			engine->game->HandleInput();
		}
	}

	KillGLWindow();
	return (msg.wParam);
}


