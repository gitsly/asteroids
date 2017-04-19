#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdlib.h>
#include "edict.h"
#include "game.h"
#include "engine.h"
#include "asteroid.h"
#include "linkclass.h"
#include "object3d.h"
#include "util.h"

CAsteroid::CAsteroid()
{
	int i;

	edict = new CEdict();

	// Initialize ships edict.
	edict->pev = this;	

	edict->mom.x = ((rand() % 64) - (rand() % 64)) / 256.0;
	edict->mom.y = ((rand() % 64) - (rand() % 64)) / 256.0;

	edict->x = (rand() % (int)engine->game->width) - (rand() % (int)engine->game->width);
	edict->y = (rand() % (int)engine->game->height) - (rand() % (int)engine->game->height);
	edict->scale = ((rand() % 16) + 16) / 24.0; 

	for( i = 0; i < 3; i++)
		mod[i] = ((rand() % 64) - (rand() % 64)) / 128.0;

	engine->edict->Add( edict );
}

CAsteroid::~CAsteroid()
{

}

void CAsteroid::Draw()
{
	if(!edict || !edict->model)
		return;

	glTranslatef( edict->x, edict->y, edict->z);
	glScalef(edict->scale, edict->scale, edict->scale);
	glRotatef(edict->angle, 0.0, 0.0, 1.0);
	glRotatef(edict->pitch, 0.0, 1.0, 0.0);
	glRotatef(edict->roll, 1.0, 0.0, 0.0);

	edict->model->Draw ( edict->frame );	
}

void CAsteroid::Think() 
{
	CEdict *c;

	edict->angle += mod[0];
	edict->pitch += mod[1];
	edict->roll += mod[2];
	
	edict->x += edict->mom.x;
	edict->y += edict->mom.y;

	c = edict->CollisionWithin(-1);
	if(c && c == engine->game->player ) // Kill player.
		c->Remove();	
		
	edict->BounceOnBorder();
}

bool CAsteroid::OnBlock() 
{ 
	return true;
}

void CAsteroid::OnRemove() 
{

}

