#include "engine.h"
#include <gl/gl.h>
#include <gl/glut.h>
#include <direct.h>
#include <string.h>
#include <stdio.h>
#include "object3d.h"
#include "linkclass.h"
#include "texture.h"
#include "light.h"
#include "mouse.h"
#include "edict.h"
#include "cbase.h"
#include "game.h"
#include "util.h"

CEngine::CEngine()
{
	game = new CGame;
	
	memset(&key, false, sizeof(key) ); 			
	active = true;		
	fullscreen = false;	

	mouse = new CMouse(hWnd, directInput, hInstance);

	// Init linked lists.
	textures = new CTextureBase;
	model = new CLink<C3dObject>;
	edict = new CLink<CEdict>;
	light = new CLink<CLight>;

	getcwd(baseDir, sizeof(baseDir) );
	strcat(baseDir, "/data");

	desiredFps = 100;

	fov = 90.0;
	xres = 640;
	yres = 480;
}

CEngine::~CEngine()
{
	Destroy (light);
	Destroy (textures);
	Destroy (model);
	Destroy (edict);
	Destroy (game);
}

void CEngine::ChangeDir(char *dir)
{
	char path[256];
	
	strcpy(path, baseDir);
	strcat(path, dir);
	chdir( path );
}

void CEngine::RawUpdate()
{
	CLinkNode<CEdict> *edict;
	CLink<CEdict> transparent; // Used to crete separate draw list for translucent edicts.

	if(!active)
		return;

	//mouse->ReadMouse();

	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw edicts.		
	edict = engine->edict->cap;
	while(edict)
	{
		glLoadIdentity();
		gluLookAt(0.0, 0.0, game->camera_z,  0.0, 0.0, 0.0, 0.0, 1.0, 0.0);      

		if(edict->data->trans < 1.0)
			transparent.Add( edict->data );
		else if(edict->data->model)
			edict->data->pev->Draw(); 
			
		edict = edict->next;
	}

	// draw transparent edicts.
	glEnable(GL_BLEND);  
	glDisable(GL_DEPTH_TEST);  
	edict = transparent.cap;
	while(edict)
	{
		glLoadIdentity();
		gluLookAt(0.0, 0.0, game->camera_z,  0.0, 0.0, 0.0, 0.0, 1.0, 0.0);      
		edict->data->pev->Draw();
		edict = edict->next;
	}
	glEnable(GL_DEPTH_TEST);  
	glDisable(GL_BLEND);  

	
	transparent.Disable();
}


void CEngine::SetupLights()
{
	CLinkNode<CLight> *l;
	int i = 0, maxLights;

	l = engine->light->cap;
	glGetIntegerv(  GL_MAX_LIGHTS, &maxLights);
	while(l && i < maxLights)
	{
		glEnable(GL_LIGHT0 + i);
		glLightfv( GL_LIGHT0 + i, GL_POSITION,  l->data->pos );
		glLightfv( GL_LIGHT0 + i, GL_DIFFUSE,  l->data->col );
		l = l->next;
		i ++;
	}
}

C3dObject* CEngine::GetModelByName(char *name)
{
	CLinkNode<C3dObject> *m;

	m = model->cap;
	while(m)
	{
		strlwr(m->data->name);
		strlwr(name);
		if(!strcmp(m->data->name, name) )
			return m->data;
		m = m->next;
	}
	return NULL;
}


