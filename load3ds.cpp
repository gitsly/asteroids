/* Filename:	load3ds.cpp
 * Author:		 Martin Collberg
 * Email:		  comayog@swipnet.se
 * Created:		00-05-12
 * Modified:   00-10-04
 *
 * Loads a 3ds file into a C3dObject class.
 * It's a member function for that class. 
 * located in C3dObject.cpp (.h)
 */

#include <fstream.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>
#include "object3d.h"
#include "texture.h"
#include "light.h"
#include "linkclass.h"
#include "engine.h"
#include "hash.h"
#include "util.h"

//Used to indent log file.
char spaces[128] = { '\0' };

static C3dObject *newObject = NULL;
static CLight *newLight = NULL;

static char currentMaterial[1024]; 
static CHashTable *mapForMat = NULL; 

void log3ds(char *fmt, ...)
{
//	return;
	
	FILE *fp;
	va_list     argptr;
	char        txt[256];
	static bool  cleared = false;	

	//clear any old log file.
	if(!cleared)
	{
		fp = fopen("3dsloadlog.txt", "w"); fclose(fp);
		cleared = true;		
	}

	va_start (argptr,fmt); // Start list after variable fmt.  va_arg(argptr, double)  get list argument into a variable in this case (double).
	vsprintf (txt,fmt,argptr);
	va_end   (argptr);

	fp = fopen("3dsloadlog.txt", "at"); //Stores new after old log...
	fwrite(&txt, strlen(txt), 1, fp);
	fclose(fp);
}

void parseVertices(istream &is, C3dObject *obj, float scale)
{
	int i;
	float v[3];
	unsigned short numVertices;

	is.read((char *)&numVertices, 2);

	//NOTE: only support one frame 3ds files, for now...
	if(!obj->frame) 
	{		
		obj->frame = new CFrame[1];
		obj->frame[0].vertex = new CVertex[numVertices];
		obj->numFrames = 1;
		obj->numVertices = numVertices;
	}

	for(i = 0; i < numVertices; i++)
	{
		is.read((char *)v, 12);
		if(	i < obj->numVertices)
		{
			obj->frame[0].vertex[i].x = v[0] * scale;
			obj->frame[0].vertex[i].y = v[1] * scale;
			obj->frame[0].vertex[i].z = v[2] * scale;
			log3ds("%s  Vertex%4i (%2.2f, %2.2f, %2.2f)\n", spaces, i, obj->frame[0].vertex[i].x, obj->frame[0].vertex[i].y, obj->frame[0].vertex[i].z);
		}
	}
}

void parseFaces(istream &is, C3dObject *obj)
{
	unsigned short n[4];
	unsigned short numFaces;
	int i, j;

	is.read((char *)&numFaces, 2);
	
	if(!obj->face) 
	{		
		obj->face = new CFace[numFaces];
		obj->numFaces  = numFaces;
	}

	log3ds("%s  found %i faces\n", spaces, numFaces);
	for(i = 0; i < numFaces; i++)
	{
		is.read((char *)n, 8); // n[3] is only crap but we still need to read it. Therefore 8 bytes.
		if( i < obj->numFaces)
		{
			obj->face[i].texture = NULL; 
			for(j=0; j<3; j++) //Find 3 vertices that makes up face. (triangle polygon).
			{
				obj->face[i].v[j] = n[j];				
				obj->face[i].t[j] = n[j];				
			}					
			log3ds("%s  Face%4d: [%d; %d; %d]\n", spaces, i, n[0], n[1], n[2]);
		}
	}
}

void parseTextureCoords(istream &is, C3dObject *obj)
{
	int i;
	float uv[2];
	unsigned short numCoords;
	
	is.read((char*)&numCoords, 2); 

	if(!obj->textureCoord)
		obj->textureCoord = new CTextureCoord[numCoords];
	obj->numTextureCoords = numCoords;

	for(i = 0; i < numCoords; i++)
	{
		is.read((char *)uv, 8);
		if(i < obj->numVertices)
		{
			obj->textureCoord[i].u = uv[0];				
			obj->textureCoord[i].v = uv[1];				
			log3ds("%s  TexCoord for vertex%4i (%2.2f, %2.2f)\n", spaces, i, uv[0], uv[1]);
		}
	}			
}

void assignMaterial(istream &is, C3dObject *obj)
{
	int i;
	char name[128];
	unsigned short num, face;	

	is.getline(name, 128,'\0'); 

	is.read((char *)&num, 2);

	for(i = 0; i < num; i++)
	{
		is.read((char *)&face, 2);
		
		obj->face[face].texture = engine->textures->GetTexture( mapForMat->GetValue(name) );

		log3ds("%s  %s assigned to face %d\n", spaces, name, face );
	}

}

//Validates that loaded object meets all the expectations.
// Then finish it.
void checkOK(C3dObject *obj)
{
	int i;


	obj->CalcSimpleNormals();

	obj->GenerateDisplayList();	

	//obj->GenerateLightNormals();
//	obj->InverseLightNormals();

	for(i = 0; i < obj->numFrames; i++)
		obj->frame[i].CalculateCenterPoint( obj->numVertices );

}

streampos filelength;

bool parseChunk(istream &is, float scale)
{
	unsigned short id;
	streampos endpos, startpos;
	unsigned long length;
	char buf[1024]; // Temporary string storage.
	float c[3];

	startpos = is.tellg();
	is.read((char *)&id, 2);
	is.read((char *)&length, 4);
	endpos = startpos + length;

	// Make sure you read (or seek away) all the data for a chunk before parsing the sub-chunks!!!, else the parser will read the wrong headers.
	
	log3ds("%s0x%04X : ", spaces, id);
	switch(id)
	{
		case 0x3d3d: // 3D editor chunk
		case 0x4d4d: // Main Chunk.
			log3ds("\n");
			strcat(spaces, "  ");
			while(is.tellg() < endpos)  // Parse Sub-Chunks.
			{			
				if(!parseChunk(is, scale) )
					break;
			}
			spaces[strlen(spaces)-2] = '\0';
			log3ds("%s...\n", spaces);
			break;

		case 0x4000: // New object.
			log3ds("new object\n");
			if(!newObject) 
				newObject = new C3dObject;
			is.getline(newObject->name, 64,'\0'); 
			//Get object's info.
			strcat(spaces, "  ");
			while(is.tellg() < endpos)  
			{			
				if(!parseChunk(is, scale))
					break;
			}
			spaces[strlen(spaces)-2] = '\0';

			checkOK(newObject);	
			log3ds("%sObject done\n", spaces);
			engine->model->Add( newObject ); //link it.
			newObject = NULL; // Done...
			is.seekg(endpos, ios::beg); // Ensure starting at next chunk.
			break;		

		case 0x4100:
			log3ds("Triangular mesh\n");
			strcat(spaces, "  ");
			while(is.tellg() < endpos)  // Parse Sub-Chunks.
			{			
				if(!parseChunk(is, scale))
					break;
			}
			spaces[strlen(spaces)-2] = '\0';
			log3ds("%sMesh done\n", spaces);
			break;				

		case 0x4110: //Vertices.
			log3ds("Reading vertices\n");
			parseVertices(is, newObject, scale);
			break;

		case 0x4120: //Faces
			log3ds("Reading faces\n");
			strcat(spaces, "  ");
			parseFaces(is, newObject);
			while(is.tellg() < endpos)  // Parse Sub-Chunks.
			{			
				if(!parseChunk(is, scale))
					break;
			}
			spaces[strlen(spaces)-2] = '\0';
			log3ds("%sFaces done\n", spaces);
			break;
			
		case 0x4130: //Faces material list
			log3ds("Assign materials\n");
			assignMaterial(is, newObject);			
			break;	

		case 0x4140: //Mapping coordinates list for each vertex			
			log3ds("Reading mapping coords\n");
			parseTextureCoords(is, newObject);
			break;

		case 0xafff: // New Material
			log3ds("new material\n");
			//Get material maps
			strcat(spaces, "  ");
			while(is.tellg() < endpos)  
			{			
				if(!parseChunk(is, scale))
					break;
			}
			spaces[strlen(spaces)-2] = '\0';
			log3ds("%sMaterial done\n", spaces);

			is.seekg(endpos, ios::beg); // Ensure starting at next chunk.
			break;		

		case 0xa000: //Material name
			is.getline(buf, 1024,'\0'); 

			strcpy(currentMaterial, buf);				

			log3ds("Name: %s\n", buf);
			break;

		case 0xa200 : //Texture map 1
			log3ds("Texture map 1\n");
			strcat(spaces, "  ");
			while(is.tellg() < endpos)  
			{			
				if(!parseChunk(is, scale))
					break;
			}
			spaces[strlen(spaces)-2] = '\0';
			log3ds("%sMap done\n", spaces);
			is.seekg(endpos, ios::beg); // Ensure starting at next chunk.
			break;		

		case 0xa300: //make new material of texture map file name 1
			is.getline(buf, 1024,'\0'); 

			mapForMat->Add(currentMaterial, buf);			
			engine->textures->GetTexture( buf );

			log3ds("mapping file: %s\n", buf);
			break;

		case 0x4600: //Light
			is.read((char *)c, 12);
			newLight = new CLight;
			newLight->SetPosition (c[0]*scale, c[1]*scale,  c[2]*scale, 1.0 );
			log3ds("Light at (%2.2f,%2.2f,%2.2f)\n", newLight->col[0], newLight->col[1], newLight->col[2]);
			//Get Maps's info.
			strcat(spaces, "  ");
			while(is.tellg() < endpos)  
			{			
				if(!parseChunk(is, scale))
					break;
			}

			engine->light->Add ( newLight );			
			newLight = NULL;
			
			spaces[strlen(spaces)-2] = '\0';
			log3ds("%sLight done\n", spaces);
			break;		
		
		case 0x0010: //Light color RGB chunk.
			is.read((char *)c, 12);				
			log3ds("Light color (%2.2f, %2.2f, %2.2f)\n", c[0], c[1], c[2]);
			newLight->SetColor( c[0], c[1] , c[2], 1.0 );
			break;
			
		case 0xB000: 
			log3ds("Keyframer chunk");
			is.seekg(endpos, ios::beg); 
			break;

		default: //Skip the chunk. 
			log3ds("skip\n");
			is.seekg(endpos, ios::beg); //ensure starting at next chunk.
			break;
	}

	if(is.tellg() >= filelength)
		return false;

	return true;
}

// Firs't loaded object goes to this. (and link) other's
// goes to linked list.
void C3dObject::Load3ds(char *file, float scale) 
{
	unsigned short data;

	if(!fopen(file, "rb")) 
		end("couldn't open file: %s", file);	

	ifstream is(file, ios::in|ios::binary);
	
	is.read((char *)&data, 2);
	if(data != 0x4d4d)
		end("%s isn't a valid 3ds file", file);

	mapForMat = new CHashTable;

	//Get filelength.
	is.seekg(0, ios::end);	
	filelength = is.tellg();
	newObject = this;
	is.seekg(0, ios::beg);	
	parseChunk(is, scale);

	if(mapForMat)
		delete mapForMat;
}
