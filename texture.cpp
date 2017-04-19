/* Filename:	texture.cpp
 * Author:		 Martin Collberg
 * Email:		  comayog@swipnet.se
 * Created:		00-10-03
 * Modified:   00-10-05
 *
 * Baseclass for the texture handling
 * and sub classes like CTexture.
 *
 */

#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>

#include "linkclass.h"
#include "bitmap.h"
#include "texture.h"
#include "util.h"

//----------------------
// Class Texture code
//----------------------

CTexture::CTexture()
{
	file = NULL;
	glStorage = -1;
}

CTexture::CTexture(char *f)
{
	char *path = getcwd(NULL, 1024);
	Bitmap bitmap;
	glStorage = 0;
	float scale;
	int	t, nw, nh;
	
	if(!bitmap.LoadBitmap( f ))
		end("couldn't load bitmap %s/%s\n", path, f);
	
	file = new char[strlen(f) + strlen(path) + 2];
	sprintf(file, "%s/%s", path, f);

	// Stretch texture into correct dimensions.
	scale = 1.0;
	t = (bitmap.h > bitmap.w) ?  bitmap.h : bitmap.w;
	if( t > 256)
		scale = 256 / (float)t;
	nw = int(bitmap.w * scale);	
	nh = int(bitmap.h * scale);	
	bitmap.Stretch(nw, nh);

/*
	t = (bitmap.h > bitmap.w) ?  bitmap.h : bitmap.w;
	nw = bitmap.w;	
	nh = bitmap.h;	
	while( t > 256 )
	{
		t /= 2;
		nw /= 2;
		nh /= 2;
	}
	bitmap.Stretch(nw, nh);
*/
	glGenTextures(1, &glStorage);
	glBindTexture(GL_TEXTURE_2D, glStorage);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, bitmap.w, bitmap.h, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap.data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  //GL_NEAREST is faster than GL_LINEAR (but non interpolated).
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

// Must be full path (used with reolad).
void CTexture::Load(char *f)
{
	Bitmap bitmap;
	glStorage = 0;
	float scale;
	int	t, nw, nh;
	
	if(!bitmap.LoadBitmap( f ))
		end("couldn't load bitmap %s\n", f);
	// Stretch texture into correct dimensions.
	scale = 1.0;
	t = (bitmap.h > bitmap.w) ?  bitmap.h : bitmap.w;
	if( t > 256)
		scale = 256 / (float)t;
	nw = int(bitmap.w * scale);	
	nh = int(bitmap.h * scale);	

	// Manual.
	if( nw == nh &&  nw == 128) 
		nw = nh = 64;

	bitmap.Stretch(nw, nh);

	glGenTextures(1, &glStorage);
	glBindTexture(GL_TEXTURE_2D, glStorage);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, bitmap.w, bitmap.h, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap.data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  //GL_NEAREST is faster than GL_LINEAR (but non interpolated).
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}



CTextureBase::CTextureBase()
{
	tbase = new CLink<CTexture>;			
}

CTextureBase::~CTextureBase()
{
	Destroy (tbase);		
}

CTexture *CTextureBase::GetTexture(char *name)
{
	CTexture *t = NULL;
	char *file;
	char *path = getcwd(NULL, 1024);

	file = new char[strlen(name) + strlen(path) + 2];
	sprintf(file, "%s/%s", path, name);
	
	// Check if already exist.
	CLinkNode<CTexture> *n = tbase->cap;
	while(n)
	{
		if(!strcmp(n->data->file, file) )		
			return n->data;
		n = n->next;
	}		
	// Load new
	t = new CTexture(name);
	tbase->Add ( t );

	return t;
}

void CTextureBase::ReloadTextures()
{
	CTexture *t = NULL;

	CLinkNode<CTexture> *n = tbase->cap;
	while(n)
	{
		n->data->Load( n->data->file );
		n = n->next;
	}		
}