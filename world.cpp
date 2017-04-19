#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include "edict.h"
#include "game.h"
#include "engine.h"
#include "world.h"
#include "linkclass.h"
#include "object3d.h"
#include "util.h"

CWorld::CWorld()
{
	edict = new CEdict();

	// Initialize ships edict.
	edict->pev = this;	

	edict->model = engine->GetModelByName( "world" );	
	if(!edict->model)
		end("couldn't assign model 'world' to edict.\n");

	engine->edict->Add( edict );
}

CWorld::~CWorld()
{
}

void CWorld::Draw()
{
	if(!edict || !edict->model)
		return;
	edict->model->Draw ( 0 );	

}

void CWorld::Think() 
{
}

void CWorld::OnRemove() 
{

}

