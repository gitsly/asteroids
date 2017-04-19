/* Filename:	object3d.cpp
 * Author:		 Martin Collberg
 * Email:		  comayog@swipnet.se
 * Created:		00-10-01
 * Modified:   00-10-04
 *
 * A Bitmap class, will load more things in the future.
 *
 */

#include <math.h>
#include <windows.h>
#include <GL/gl.h>

#include "object3d.h"
#include "texture.h"
#include "util.h"

//----------------------
// Class Vertex code
//----------------------

CVertex::CVertex()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

CVertex::CVertex(float x1, float y1, float z1)
{
	x = x1;
	y = y1;
	z = z1;
}

void CVertex::Set(float x1, float y1, float z1)
{
	x = x1;
	y = y1;
	z = z1;
}

//----------------------
// Class Face code
//----------------------
CFace::CFace()
{
	texture = NULL;
}

CFace::~CFace()
{
	
}


//----------------------
// Class Frame code
//----------------------

CFrame::CFrame()
{
	vertex = NULL;	
}

CFrame::~CFrame()
{
	DestroyArray ( vertex ); 
}

Vector3 CFrame::CalculateCenterPoint( int numVertices)
{
	int i;

	for( i = 0; i <  numVertices;  i++)
		center += vertex[i];  	
	center /= (float)numVertices;
	
	return center;
}

//-----------------------
// Class 3dObject code.
//-----------------------
C3dObject::C3dObject ()
{
	face = NULL;
	frame = NULL;
	textureCoord = NULL;
	numFrames = 0;
	numVertices = 0;
	numFaces = 0;
}

C3dObject::C3dObject (int numFaces1, int numVertices1 ,int numFrames1)
{
	int i;		

	numVertices = numVertices1;
	numFrames = numFrames1;
	numFaces = numFaces1;
	numTextureCoords = numVertices;

	face = new CFace[numFaces];
	frame = new CFrame[numFrames];
	for(i = 0; i < numFrames; i++)
		frame[i].vertex = new CVertex[numVertices];
	textureCoord = new CTextureCoord[numTextureCoords];
}


C3dObject::~C3dObject ()
{
	DestroyArray(face);
	DestroyArray(frame);
	DestroyArray(textureCoord);
}

void C3dObject::Draw(int frm)
{
	glCallList(frame[frm].displayList);
}


// Normals should have been calculated before this one.
void C3dObject::GenerateDisplayList()
{
	#define vrt (frame[frm].vertex[ face[i].v[j] ])
	int i, j, frm;

	for( frm = 0; frm < numFrames; frm ++)
	{
		frame[frm].displayList = glGenLists(1);	
		glNewList( frame[frm].displayList, GL_COMPILE);

		for(i = 0; i < numFaces; i++)
		{
			if(face[i].texture)
				glBindTexture(GL_TEXTURE_2D, face[i].texture->glStorage);
			glBegin(GL_POLYGON);
			for(j = 0; j < 3; j++)
			{
				if(face[i].texture)
					glTexCoord2f(textureCoord[face[i].t[j]].u, textureCoord[face[i].t[j]].v);
				glNormal3f( vrt.n.x, vrt.n.y, vrt.n.z );
				glVertex3f( vrt.x,  vrt.y, vrt.z );
			}
			glEnd();
		}
		glEndList();
	}
}

void C3dObject::CalcSimpleNormals()
{
	int a, b, c, fc, frm;
	float l;
	Vector3 vu, vv, normal;

	for(fc = 0; fc < numFaces; fc ++)
	{
		for(frm = 0; frm < numFrames; frm ++)
		{
			a = face[fc].v[0];
			b = face[fc].v[1];
			c = face[fc].v[2];

			vu = frame[frm].vertex[a] - frame[frm].vertex[b];
			vv = frame[frm].vertex[c] - frame[frm].vertex[b];

			normal = crossProduct(vu, vv);
			l = (float)sqrt( (normal.x*normal.x) + (normal.y*normal.y) + (normal.z*normal.z) );
			normal.x /= -l;
			normal.y /= -l;
			normal.z /= -l;

			for( a = 0; a < 3; a++)
				frame[frm].vertex[ face[fc].v[a] ].n = normal;
		}
	}
}

void C3dObject::MakeFacetNormal (int facenum, int frm, float *fn)
{
	float v1[3], v2[3];
	v1[0] = frame[frm].vertex[face[facenum].v[1]].x - frame[frm].vertex[face[facenum].v[0]].x;
	v1[1] = frame[frm].vertex[face[facenum].v[1]].y - frame[frm].vertex[face[facenum].v[0]].y;
	v1[2] = frame[frm].vertex[face[facenum].v[1]].z - frame[frm].vertex[face[facenum].v[0]].z;
	v2[0] = frame[frm].vertex[face[facenum].v[2]].x - frame[frm].vertex[face[facenum].v[0]].x;
	v2[1] = frame[frm].vertex[face[facenum].v[2]].y - frame[frm].vertex[face[facenum].v[0]].y;
	v2[2] = frame[frm].vertex[face[facenum].v[2]].z - frame[frm].vertex[face[facenum].v[0]].z;
	fn[0] = v1[1] * v2[2] - v1[2] * v2[1];
	fn[1] = v1[2] * v2[0] - v1[0] * v2[2];
	fn[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void C3dObject::GenerateLightNormals ()
{
	int i, j, k;
	float facetnormal[3];

	for (i = 0; i < numFrames; i++)
	{
		// clear all normals 
		for (j=0; j < numVertices; j++)
		{
			frame[i].vertex[j].n.x = 0.0;
			frame[i].vertex[j].n.y = 0.0;
			frame[i].vertex[j].n.z = 0.0;
		}
		// calc normals 
		for (j = 0; j < numFaces; j++)
		{
			MakeFacetNormal (j, i, facetnormal);
			for (k = 0; k < 3; k++)
			{
				frame[i].vertex[face[j].v[k]].n.x -= facetnormal[0];
				frame[i].vertex[face[j].v[k]].n.y += facetnormal[2];
				frame[i].vertex[face[j].v[k]].n.z -= facetnormal[1];
			}
		}

		// normalize normals 
		for (j = 0; j < numVertices; j ++)
			frame[i].vertex[j].n.normalize();
	}
}

void C3dObject::InverseLightNormals()
{
	int i, j;
	for(i = 0; i < numFrames; i++)
		for(j = 0; j < numVertices; j++)
			frame[i].vertex[j].n.inverse();
}



/*
#include <math.h>

#include "main.h"

#include "object3d.h"
#include "vector.h"

Object3d::~Object3d() 
{
	if(face)
		delete face;
	if(frame)
		delete[] frame;
	if(texCoord)
		delete texCoord;
	if(next)
		delete next;
}

void Object3d::makeFacetNormal (int facenum, int frm, float *fn)
{
	float v1[3], v2[3];
	v1[0] = frame[frm].v[face[facenum].v[1]].x - frame[frm].v[face[facenum].v[0]].x;
	v1[1] = frame[frm].v[face[facenum].v[1]].y - frame[frm].v[face[facenum].v[0]].y;
	v1[2] = frame[frm].v[face[facenum].v[1]].z - frame[frm].v[face[facenum].v[0]].z;
	v2[0] = frame[frm].v[face[facenum].v[2]].x - frame[frm].v[face[facenum].v[0]].x;
	v2[1] = frame[frm].v[face[facenum].v[2]].y - frame[frm].v[face[facenum].v[0]].y;
	v2[2] = frame[frm].v[face[facenum].v[2]].z - frame[frm].v[face[facenum].v[0]].z;
	fn[0] = v1[1] * v2[2] - v1[2] * v2[1];
	fn[1] = v1[2] * v2[0] - v1[0] * v2[2];
	fn[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void Object3d::generateLightNormals ()
{
	int i, j, k;
	float facetnormal[3];

	for (i=0; i<numFrames; i++)
	{
		// clear all normals 
		for (j=0; j<numPoints; j++)
		{
			frame[i].v[j].normal.x = 0.0;
			frame[i].v[j].normal.y = 0.0;
			frame[i].v[j].normal.z = 0.0;
		}
		// calc normals 
		for (j=0; j<numFaces; j++)
		{
			makeFacetNormal (j, i, facetnormal);
			for (k=0; k<3; k++)
			{
				frame[i].v[face[j].v[k]].normal.x -= facetnormal[0];
				frame[i].v[face[j].v[k]].normal.y += facetnormal[2];
				frame[i].v[face[j].v[k]].normal.z -= facetnormal[1];
			}
		}

		// normalize normals 
		for (j=0; j<numPoints; j++)
			frame[i].v[j].normal.normalize();
	}
}

void Object3d::inverseLightNormals()
{
	int i, j;
	for(i=0; i<numFrames; i++)
		for(j=0; j<numPoints; j++)
			frame[i].v[j].normal.inverse();
}

// -------------
// Frame code
// -------------

Frame::~Frame()
{
	if(v)
		delete v;
}


// -------------
// Map code
// -------------
//Creates a map from a bitmap.

Map::Map()
{
	type = mat_diffuse;
	strcpy( mappingFilename	, "");
	glStorage = 0;
	next = NULL;
}

void Map::LoadBmp(char *path, char *file)
{
	char *filepath;
	AUX_RGBImageRec *holder;

	holder = NULL;
	glStorage = 0;

	if (file)
	{
		filepath = new char[strlen(file)+strlen(path)+1];
		strcpy(filepath, path);		
		strcat(filepath, file);
		holder = auxDIBImageLoad(filepath);
		delete filepath;
	}
	if(!holder)
		end("aborting\n");

	strcpy( mappingFilename	, file);

	glGenTextures(1, &glStorage);
	glBindTexture(GL_TEXTURE_2D, glStorage);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, holder->sizeX, holder->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, holder->data);
	//Set min and max stretch filtering.
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  //GL_NEAREST is faster than GL_LINEAR (but non interpolated).
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if(holder->data)
		delete holder->data;
	delete holder;
}

Map::~Map()
{
	glStorage = 0;
	if(next)	
		delete next;
}

void Map::use()
{
	//Later do stuff with different types of maps.
	glBindTexture(GL_TEXTURE_2D, glStorage);
}

// -------------
// Material code
// -------------

Material::Material()
{
	maps = NULL;
	strcpy( materialName	,"");
}

Material::~Material()
{
	if(maps)
		delete maps;
	if(next)
		delete next;
}


//--------------
// Faces code
//--------------

void Face::setMaterial(char *name)
{
	Material *link;	
	link = scene->matcap;
	while(link)
	{
		if(!strcmp(link->materialName, name))
		{
			material = link;			
			return;			
		}
		link = link->next;
	}
	end("Requested material '%s' couldn't be found", name);
}

//--------------
// Light code
//--------------
int Light::numLights = 0;

Light::Light()
{
	end("invalid constuctor: Light()");
}

Light::~Light()
{
	delete next;
}

Light::Light(float x1, float y1, float z1)
{
	pos[0] = x1;
	pos[1] = y1;
	pos[2] = z1;
	pos[3] = 1.0;
	rgba[0] = 1.0;
	rgba[1] = 1.0;
	rgba[2] = 1.0;
	rgba[3] = 1.0;

	ambient = false;
	diffuse = true;

	id = GL_LIGHT0 + numLights;

	if(diffuse)
		glLightfv(id, GL_DIFFUSE , rgba);
	if(ambient)
		glLightfv(id, GL_AMBIENT , rgba);
	glLightfv(id, GL_POSITION , pos);
	
	glEnable(id);

	
	numLights ++;
	if(numLights>=8)
		end("%i lights is maximum", numLights);

}

void Light::setColor(float r, float g, float b)
{
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = 1.0;
	if(diffuse)
		glLightfv(id, GL_DIFFUSE , rgba);
	if(ambient)
		glLightfv(id, GL_AMBIENT , rgba);
}


//-----------
// Camera
//-----------

#define RAD(x)  (x*M_PI/180.0)

Vector3 Camera::getViewVector()
{
	Vector3 v;
	v.x = cos(RAD(angle));
	v.z = sin(RAD(angle));
	v.y = tan(RAD(pitch));			
	return v;
}

void Camera::view()
{
	Vector3 r; //reference center point.

	r = getViewVector();		
	gluLookAt(x, y, z, x+r.x, y+r.y, z+r.z,  0.0, -1.0, 0.0);
}

*/