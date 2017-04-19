/* Filename:	bitmap.cpp
 * Author:		 Martin Collberg
 * Email:		  comayog@swipnet.se
 * Created:		00-09-32
 * Modified:   00-10-03
 *
 * A Bitmap class, will load more formats in the future.
 * will also have more functions.
 */

#include <GL/glaux.h>	
#include <fstream.h>
#include <stdio.h>
#include "bitmap.h"
#include "util.h"

Bitmap ::~Bitmap()
{
	if(data)
		delete data;
	data = NULL;
}


//----------------------------
// Read image files functions
//----------------------------

bool Bitmap::LoadBitmap(char *file)
{
	char ext[5];
	int i, l =	strlen(file);

	for(i = l - 3; (i < l && l > 3); i++)
		ext[i - (l - 3)] = file[i];
	ext[i - (l - 3)] = '\0';

	strlwr(ext);
	if(!strcmp(ext, "pcx"))
		return LoadPcx(file);
	if(!strcmp(ext, "bmp"))
		return LoadBmp(file);
	return false;
}

bool Bitmap::LoadBmp(char *file)
{
	AUX_RGBImageRec *holder;
	holder = auxDIBImageLoad(file);
	if(!holder)
		end("Couldn't open Bmp image: %s", file);

	w = holder->sizeX;
	h = holder->sizeY;
	data = new Rgb[w * h];
	memcpy(data, holder->data, w * h * sizeof(Rgb));

	if(holder->data)
		delete holder->data;
	delete holder;

	return true;
}

bool Bitmap::LoadPcx(char *file)
{
	unsigned char pix, npcx, col, header[128], *colnum;
	short xmax,ymax,ymin,xmin;
	long x, size, i;
	palette pal;

	if(!fopen(file, "rb"))
		end("Couldn't open Pcx image: %s", file);

	ifstream is(file, ios::in|ios::binary);
	is.read(header, 128);
    xmin=header[4]+header[5] * 256;
    ymin=header[6]+header[7] * 256;
    xmax=header[8]+header[9] * 256;
    ymax=header[10]+header[11] * 256;
	w=xmax-xmin+1;
	h=ymax-ymin+1;
	size = w*h;
	data = new Rgb[size];
	colnum = new unsigned char[size];
	x = 0;
	
	while(x < size)	
	{
		is.read(&pix, 1);
		if (pix>192)
        {
			npcx = pix-192;
			is.read(&col, 1);
		}
		else
        {
			col=pix;
			npcx=1;
		}
        for(i=0; i< npcx; i++) 
			colnum[x+i]=col;
		x += npcx;
	}
	//read palette.
    is.seekg(-768, ios::end);
    is.read((char *)&pal, 768);
	for(i = 0; i < size; i++) //Assign colors to data.
	{
//		end("(%i, %i, %i)", pal[15].r, pal[15].g, pal[15].b);
		data[i].r = pal[colnum[i]].r;
		data[i].b = pal[colnum[i]].b;
		data[i].g = pal[colnum[i]].g;
	}
	is.close();
	delete colnum;
	return true;
}

//----------------------------
// Operations.
//----------------------------
void Bitmap::Stretch(int nw, int nh)
{
	Rgb *dest;
	float xr, yr, x1, y1;
	int x, y;		


	if(!data)
		return;
	dest = new Rgb[nw*nh];
	xr = (float)w /nw;  					
	yr = (float)h / nh;  					
	for(x = 0; x < nw; x++)	
	{
		for(y = 0; y < nh; y++)	
		{
			x1 = x * xr;
			y1 = y * yr;
			dest[x + (y * nw)] = data[int(x1 + (y1*w)) ];
		}
	}
	delete data;
	data = dest;
	w = nw;
	h = nh;
}
