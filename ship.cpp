#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include "edict.h"
#include "game.h"
#include "engine.h"
#include "ship.h"
#include "laser.h"
#include "particle.h"
#include "linkclass.h"
#include "object3d.h"
#include "util.h"

CShip::CShip()
{
	edict = new CEdict();

	// Initialize ships edict.
	edict->pev = this;	

	speed = 0.002;
	maxSpeed = 0.25;
	frictionMod = 0.995;

	edict->model = engine->GetModelByName( "ship" );	
	edict->CalcRadius();		

	edict->x = 0;
	edict->y = -15;

	thrust = false;

	engine->edict->Add( edict );
}

CShip::~CShip()
{
}

void CShip::Draw()
{
	if(!edict || !edict->model)
		return;

	glTranslatef( edict->x, edict->y, edict->z);
	glRotatef(edict->angle, 0.0, 0.0, 1.0);
	glRotatef(edict->pitch, 0.0, 1.0, 0.0);
	glRotatef(edict->roll, 1.0, 0.0, 0.0);

	edict->model->Draw ( edict->frame );	
}

void CShip::Think() 
{
	CFadeParticle *p;

	edict->x += edict->mom.x;
	edict->y += edict->mom.y;

	edict->mom *= frictionMod;

	if(edict->mom.length() > maxSpeed)
		edict->mom = edict->mom.normalize() * maxSpeed;

	edict->BounceOnBorder();

	if( thrust)
	{
		p = new CFadeParticle;
		p->edict->model = engine->GetModelByName("splot");
		p->color[0] = 1.0;	p->color[1] = 0.01;	p->color[2] = 0.01;
		p->edict->mom = - Vector3( cos(edict->angle / (180 /M_PI)), sin(edict->angle / (180 /M_PI)), 0.0 );
		p->edict->CopyPosition( edict );
		p->edict->x += p->edict->mom.x;
		p->edict->y += p->edict->mom.y;
		p->edict->mom /= 10;
		p->edict->trans = 0.25;
		p->edict->scale = 0.4 /  float(rand() % 3);
		p->colorChange[0] = 1.8;  p->colorChange[1] = 1.8; p->colorChange[2] = 0.2;
		p->friction = 0.99;
		p->fadeRate = 0.95;
		p->scaleRate = 0.95;
		p->life = 50;
		thrust = false;
	}

}


void CShip::OnFire()
{
	CLaser *laser;
	Vector3 v( cos(edict->angle / (180/M_PI)) ,   sin(edict->angle / (180/M_PI)),   0.0 );

	laser = new CLaser;
	laser->edict->mom = v;
	laser->edict->mom *= 2.5;
	laser->edict->trans = 0.25;
	laser->edict->angle = edict->angle;
	laser->edict->CopyPosition( edict );
	laser->edict->scale = 0.35;
	laser->color[0]	= 1.0;	
	laser->color[1]	= 1.0;	
	laser->color[2]	= 0.1;	
}

void CShip::OnMoveForward() 
{
	thrust = true;
	edict->mom.x += speed * cos(edict->angle * M_PI / 180 );
	edict->mom.y += speed * sin(edict->angle * M_PI / 180 );

}

void CShip::OnMoveBackward() 
{
	edict->mom.x -= speed * cos(edict->angle * M_PI / 180 );
	edict->mom.y -= speed * sin(edict->angle * M_PI / 180 );
}


bool CShip::OnBlock() 
{ 
	return true;
}

void CShip::OnRemove() 
{
	CFadeParticle *p;
	int i;
	// MAKEME: reduce life's etc.. check gameover.
	engine->game->player = NULL;

	for( i = 0; i < 50; i ++)
	{
		p = new CFadeParticle;
		p->edict->model = engine->GetModelByName("splot");
		p->color[0] = 1.0;	p->color[1] = 1.0;	p->color[2] = 0.5;
		p->edict->mom += edict->mom;
		p->edict->mom *= 0.1;
		p->edict->CopyPosition( edict );
		p->edict->trans = 0.15;
		p->edict->scale = 0.6;
		p->friction = 0.997;
		p->fadeRate = 0.995;
		p->scaleRate = 0.97;
		p->life = 500;
	}

	for( i = 0; i < 10; i ++)
	{
		p = new CFadeParticle;
		p->edict->model = engine->GetModelByName("splot");
		p->color[0] = 1.0;	p->color[1] = 0.8;	p->color[2] = 0.9;
		p->edict->mom += edict->mom;
		p->edict->mom *= 0.3;
		p->edict->CopyPosition( edict );
		p->edict->trans = 0.15;
		p->edict->scale = 0.6;
		p->friction = 0.997;
		p->fadeRate = 0.999;
		p->scaleRate = 0.95;
		p->life = 800;
	}

	p = new CFadeParticle;
	p->edict->model = engine->GetModelByName("flare1");
	p->color[0] = 1.0;	p->color[1] = 1.0;	p->color[2] = 1.0;
	p->edict->mom = 0;
	p->edict->CopyPosition( edict );
	p->edict->trans = 0.99;
	p->edict->scale = 2.0;
	p->friction = 0.997;
	p->fadeRate = 0.99;
	p->scaleRate = 0.998;
	p->life = 1000;

}

