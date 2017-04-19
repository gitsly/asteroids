#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdlib.h>
#include "edict.h"
#include "game.h"
#include "engine.h"
#include "particle.h"
#include "asteroid.h"
#include "laser.h"
#include "linkclass.h"
#include "object3d.h"
#include "util.h"

CLaser::CLaser()
{
	edict = new CEdict();

	// Initialize ships edict.
	edict->pev = this;	

	edict->model = engine->GetModelByName( "laser" );	

	edict->scale = 1.0;
	edict->trans = 0.95;
	color[0] = 1.0;
	color[1] = 1.0;
	color[2] = 1.0;

	engine->edict->Add( edict );
}

CLaser::~CLaser()
{

}

void CLaser::Draw()
{
	if(!edict || !edict->model)
		return;

	glTranslatef( edict->x, edict->y, edict->z);
	glScalef(edict->scale, edict->scale, edict->scale);
	glRotatef(edict->angle, 0.0, 0.0, 1.0);

	glDisable(GL_LIGHTING);
	glColor4f(color[0], color[1], color[2], edict->trans);
	edict->model->Draw ( edict->frame );	
	glEnable(GL_LIGHTING);

}

void CLaser::Think() 
{
	int i;
	CEdict *c;
	CFadeParticle *p;
	CAsteroid *a;
	char model[256];

	edict->x += edict->mom.x;
	edict->y += edict->mom.y;

	c = edict->CollisionWithin( 0 );
	if(c && c != engine->game->player) // It's a hit, spawn particles and remove victim.
	{
		for( i = 0; i < 20; i ++)
		{
			p = new CFadeParticle;
			p->edict->model = engine->GetModelByName("splot");
			p->color[0] = 1.0;	p->color[1] = 1.0;	p->color[2] = 0.5;
			p->edict->mom += edict->mom + c->mom;
			p->edict->mom *= 0.06;
			p->edict->CopyPosition( c );
			p->edict->trans = 0.15;
			p->edict->scale = 0.6;
			p->friction = 0.997;
			p->fadeRate = 0.995;
			p->scaleRate = 0.97;
			p->life = 300;
		}
		p = new CFadeParticle;
		p->edict->model = engine->GetModelByName("flare1");
		p->color[0] = 1.0;	p->color[1] = 1.0;	p->color[2] = 1.0;
		p->edict->mom = 0;
		p->edict->CopyPosition( edict );
		p->edict->trans = 0.99;
		p->edict->scale = 1.0;
		p->friction = 1.0;
		p->fadeRate = 0.93;
		p->scaleRate = 0.998;
		p->life = 200;

		// Shrink or remove hit asteroids.
		c->scale *= 0.5;

		strcpy(model, c->model->name);
		if(!strcmp(model, "asteroid2"))	strcpy(model, "asteroid3");
		else strcpy(model, "asteroid2");

		if(	!strcmp(c->model->name, "asteroid3") )
			c->Remove();
		else 
		{
			c->model = engine->GetModelByName(model);		
			c->CalcRadius();

			a = new CAsteroid;
			a->edict->model = engine->GetModelByName(model);		
			a->edict->x = c->x;
			a->edict->y = c->y;
			a->edict->scale = c->scale;
			a->edict->CalcRadius();
		}
		
		edict->Remove();
	}			

	RemoveOnBorder();
}

bool CLaser::OnBlock() 
{ 
	return false;
}

void CLaser::OnRemove() 
{

}

