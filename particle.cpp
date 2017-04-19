#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdlib.h>
#include "edict.h"
#include "game.h"
#include "engine.h"
#include "particle.h"
#include "linkclass.h"
#include "object3d.h"
#include "util.h"

CParticle::CParticle()
{

	edict = new CEdict();

	// Initialize ships edict.
	edict->pev = this;	
	edict->scale = 1.0;
	edict->trans = 0.99;

	edict->mom.x = ((rand() % 64) - (rand() % 64)) / 256.0;
	edict->mom.y = ((rand() % 64) - (rand() % 64)) / 256.0;
	edict->mom.z = ((rand() % 64) - (rand() % 64)) / 256.0;
	edict->mom.normalize();

	friction = 0.995;		

	color[0] = 1.0;
	color[1] = 1.0;
	color[2] = 1.0;
	life = 20 + (rand() % 15); // About a second default life.
	engine->edict->Add( edict );
}

CParticle::~CParticle()
{

}

void CParticle::Draw()
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

void CParticle::Think() 
{
	edict->x += edict->mom.x;
	edict->y += edict->mom.y;
	edict->mom += move;

	edict->angle += 0.1;

	edict->mom *= friction;

	if(life > 0) 
	{
		life--;
		if(!life)
			edict->Remove();
	}
	OnAge();

	RemoveOnBorder();
}

bool CParticle::OnBlock() 
{ 
	return false;
}

//----------------
// CFadeParticle
//----------------

CFadeParticle::CFadeParticle()
{
	for( int i = 0; i < 3; i++)
		colorChange[i] = 1.0;
	fadeRate = 0.95;
	scaleRate = 1.0;
}

void CFadeParticle::OnAge()
{
	Vector3 col(color[0], color[1], color[2]);		

	edict->trans *= fadeRate;	
	edict->scale *= scaleRate;	
	col *= colorChange;
//	col.normalize();
	col.array( color );
}
