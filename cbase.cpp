#include <stdio.h>
#include "linkclass.h"
#include "engine.h"
#include "game.h"
#include "cbase.h"
#include "edict.h"
#include "util.h"

CBaseObject::~CBaseObject() 
{ 
};

void CBaseObject::RemoveOnBorder()
{
	if(edict->x > engine->game->width / 2 || edict->x < -engine->game->width / 2 || edict->y > engine->game->height / 2 ||	edict->y < -engine->game->height / 2)
		edict->Remove();	
}

