#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "engine.h"
#include "game.h"
#include "ship.h"
#include "world.h"
#include "asteroid.h"
#include "laser.h"
#include "object3d.h"
#include "edict.h"
#include "texture.h"
#include "linkclass.h"
#include "util.h"

CGame::CGame()
{
	ticks = 0;
	player = NULL;
}

CGame::~CGame()
{

}

void CGame::Think()
{
	CLinkNode<CEdict> *next, *edict = engine->edict->cap;

	while(edict)
	{
		edict->data->pev->Think();
		edict = edict->next;
	}

	// Delete edict loop, the ones that got the delete signal will be deleted.
	edict = engine->edict->cap;
	while(edict)
	{
		next = edict->next; 
		if(edict->data->deleted)
			engine->edict->Remove( edict->data );
		edict = next;
	}

}

// When player has died and respawning.
void CGame::Respawn()
{
	int i;
	CAsteroid  *a;

	// Clear old edicts.
	engine->edict->Clear();
	player = (new CShip)->edict;	
	world = (new CWorld)->edict;
	for( i = 0; i < 5; i ++)
	{
		a = new CAsteroid;
		a->edict->model = engine->GetModelByName("asteroid1");
		a->edict->CalcRadius();
	}
}

void CGame::LoadModels()
{
	C3dObject *o;

	o = new C3dObject;	
	engine->ChangeDir("/ship/");
	o->Load3ds("ship.3ds",  1.0);	

	o = new C3dObject;
	engine->ChangeDir("/asteroid/");
	o->Load3ds("asteroid.3ds", 1.0);	

	o = new C3dObject;
	engine->ChangeDir("/laser/");
	o->Load3ds("laser.3ds", 1.0);	

	o = new C3dObject;
	engine->ChangeDir("/particles/");
	o->Load3ds("particles.3ds", 1.0);	

	o = new C3dObject;
	engine->ChangeDir("/world/");
	o->Load3ds("world.3ds", 1.0);	

}

// Done each new level.
void CGame::Init()
{

	srand(time(0));
	camera_z = 40;

	// MAKEME: Calculate these correctly, they have to do with fov and camera_z and .
	width = 102;
	height = 78;

	LoadModels();
	engine->SetupLights();

	Respawn();
}

void CGame::HandleInput()
{
	static int fired = 0;
	
	CLinkNode<C3dObject> *model;
	bool *key = engine->key;
	
	if(player)
	{
		if(key[VK_UP])
			player->pev->OnMoveForward();
		if(key[VK_DOWN])
			player->pev->OnMoveBackward();
		if(key[VK_RIGHT])
			player->angle -= 1.5;
		if(key[VK_LEFT])
			player->angle += 1.5;

		if(key[VK_CONTROL] && !fired)
		{
			player->pev->OnFire();
			fired = 50;
		}
	}
	else if(key[VK_SPACE]) // Hit space to respawn if dead.
		Respawn();

	if (key[VK_F1])	
	{
		key[VK_F1]=false;	
		KillGLWindow();
		engine->fullscreen = !engine->fullscreen;	
		if (!CreateGLWindow( appTitle, engine->xres, engine->yres, 16, engine->fullscreen))
			end("Couldn't set the requested video mode\n");
		engine->textures->ReloadTextures();

		model = engine->model->cap;
		while(model)
		{
			model->data->GenerateDisplayList();
			model = model->next;
		}				
		engine->SetupLights();
		engine->timer = SetTimer(hWnd, engine->timer, 1000.0 / engine->desiredFps, NULL);
	
	}

	if(fired) fired --;
}
