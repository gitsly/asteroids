#include <fstream.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "object3d.h"
#include "linkclass.h"
#include "loadmd2.h"
#include "texture.h"
#include "engine.h"
#include "bitmap.h"
#include "util.h"

void logmd2(char *fmt, ...)
{
	return;
	
	FILE *fp;
	va_list     argptr;
	char        txt[256];
	static bool  cleared = false;	

	//clear any old log file.
	if(!cleared)
	{
		fp = fopen("md2log.txt", "w"); fclose(fp);
		cleared = true;		
	}
	
	va_start (argptr,fmt); // Start list after variable fmt.  va_arg(argptr, double)  get list argument into a variable in this case (double).
	vsprintf (txt,fmt,argptr);
	va_end   (argptr);

	fp = fopen("md2log.txt", "at"); //Stores new after old log...
	fwrite(&txt, strlen(txt), 1, fp);
	fclose(fp);
}

void C3dObject::LoadMd2(char *name, char *skinfile, float scale)
{
	int i, j;
	Md2Frame *frm;		
	Md2Triangle *faces;
	Md2TexCoord *texCoord;			
	Md2Header header;
	char file[1024];

	strcpy(this->name, name);
	sprintf(file, "%s.md2", name);

	if(!fopen(file, "rb")) 
		end("couldn't open file: %s", file);	

	ifstream is(file, ios::in|ios::binary);


	is.read((char *)&header, sizeof(header));

	//load texture coords.
	texCoord = new Md2TexCoord[header.numTexCoords];
	is.seekg(header.offsetTexCoords, ios::beg);
	is.read((char *)texCoord, header.numTexCoords * sizeof(Md2TexCoord));	

	//load faces data
	faces = new Md2Triangle[header.numTriangles];			
	is.seekg(header.offsetTriangles, ios::beg);
	is.read((char *)faces, header.numTriangles * sizeof(Md2Triangle));	

	numFrames = header.numFrames;
	numFaces = header.numTriangles;
	numVertices = header.numVertices;
	numTextureCoords = header.numTexCoords;

	frame = new CFrame[numFrames];			
	face = new CFace[numFaces];
	frm = new Md2Frame;
	frm->vertices = new Md2ReadTriangleStruct[numVertices];

	//Read the frame data.
	is.seekg(header.offsetFrames, ios::beg);
	for(i = 0; i < numFrames; i++)
	{
		logmd2("Frame no %i\n\n", i);

		is.read((char *)frm, 40);

		frame[i].vertex = new CVertex[numVertices];				
		for( j = 0; j < numVertices; j++)
		{
			is.read((char *)&frm->vertices[j], sizeof(Md2ReadTriangleStruct));
			frame[i].vertex[j].x = (float) ((int) frm->vertices[j].vertex[0]) * scale * frm->scale[0] + frm->translate[0];
			frame[i].vertex[j].y = (float) ((int) frm->vertices[j].vertex[1]) * scale * frm->scale[1] + frm->translate[1];
			frame[i].vertex[j].z = (float) ((int) frm->vertices[j].vertex[2]) * scale * frm->scale[2] + frm->translate[2];

			logmd2("vertex %i (%2.2f, %2.2f, %2.2f)\n", j, frame[i].vertex[j].x, frame[i].vertex[j].y, frame[i].vertex[j].z);
		}
		logmd2("End of frame\n\n");
	}

	//Assign vertices to faces and set texture coords.
	CTexture *tex =	face[i].texture = engine->textures->GetTexture( skinfile );
	for(i = 0; i < numFaces; i++)
	{
		for(j = 0; j < 3; j++)
		{
			face[i].v[2-j] = faces[i].vertex[j];
			face[i].t[2-j] = faces[i].texture[j];
		}
		face[i].texture = tex;
	}

	textureCoord = new CTextureCoord[numTextureCoords];
	for(i = 0; i < numTextureCoords; i++)
	{
		textureCoord[i].u = (float)texCoord[i].u / (float)header.skinWidth ;
		textureCoord[i].v = (float)texCoord[i].v / (float)header.skinHeight;
		logmd2("texture coord %i (%2.2f, %2.2f)\n", i, textureCoord[i].u, textureCoord[i].v);
	}		

	GenerateLightNormals();

	engine->model->Add ( this );
	
	is.close();

	delete frm->vertices; 	
	delete frm;
	delete faces;
	delete texCoord;
}








