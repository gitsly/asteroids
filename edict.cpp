#include <stdio.h>
#include "linkclass.h"
#include "engine.h"
#include "object3d.h"
#include "game.h"
#include "cbase.h"
#include "edict.h"
#include "util.h"

CEdict::CEdict()
{
	pitch = 0;
	roll = 0;
	angle = 0;
	trans = 1.0;
	frame = 0;
	scale = 1.0;
	radius = 1.0;

	deleted = false;
	pev = NULL;
	model = NULL;
}

CEdict::~CEdict()
{
	if(pev)
		pev->OnRemove();

	Destroy(pev);
}


void CEdict::CalcRadius()
{
	int i;
	float l;
	if(!model)
	{
		radius = 1.0;
		return;
	}

	radius = 0;
	for(i = 0; i < model->numVertices; i++)
	{
		l = model->frame[0].vertex[i].length();	
		if(l > radius)
			radius = l;
	}
	
	radius *= scale;
}

void CEdict::CopyPosition( CEdict *e)
{
	x = e->x;
	y = e->y;
	z = e->z;
}


void CEdict::LoopIfBeyondBorder()
{
	if(x > engine->game->width / 2)
		x -= engine->game->width;
	if(x < -engine->game->width / 2)
		x += engine->game->width;
	if(y > engine->game->height / 2)
		y -= engine->game->height;
	if(y < -engine->game->height / 2)
		y += engine->game->height;
}

void CEdict::BounceOnBorder()
{
	if(x > engine->game->width / 2)
	{
		mom.x = -mom.x;
		x = engine->game->width / 2;
	}
	if(x < -engine->game->width / 2)
	{
		mom.x = -mom.x;
		x = -engine->game->width / 2;
	}

	if(y > engine->game->height / 2)
	{
		mom.y = -mom.y;
		y = engine->game->height / 2;
	}
	if(y < -engine->game->height / 2)
	{
		mom.y = -mom.y;
		y = -engine->game->height / 2;
	}
}

// Returns first edict within 'dist'
// Checks pure x, y, z coords.
CEdict* CEdict::CollisionWithin(float dist)
{
	Vector3 v1, v2;
	CLinkNode <CEdict> *edict = engine->edict->cap;

	v1 = (Vector3)*this;
	while( edict )
	{
		v2 = (Vector3)*edict->data;

		float l = (v1 - v2).length(); 
		if( l <=  dist + (edict->data->radius + this->radius) && edict->data != this && edict->data->pev->OnBlock() )
			return edict->data;
		edict = edict->next;
	}

	return NULL;
}

void CEdict::Remove()
{
	deleted = true;		
}
