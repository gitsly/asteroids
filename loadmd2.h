//Loads a md2 file into an Object3d.
#ifndef __loadmd2_h__
#define __loadmd2_h__

typedef unsigned char byte;


class Md2Header
{
	public:
		int magic; 
		int version; 
		int skinWidth; 
		int skinHeight; 
		int frameSize; 
		int numSkins; 
		int numVertices; 
		int numTexCoords; 
		int numTriangles; 
		int numGlCommands; 
		int numFrames; 
		int offsetSkins; 
		int offsetTexCoords; 
		int offsetTriangles; 
		int offsetFrames; 
		int offsetGlCommands; 
		int offsetEnd; 
};

typedef struct
{
   byte vertex[3];
   byte lightNormalIndex;
} Md2ReadTriangleStruct;

class Md2TexCoord
{
	public:
		short u, v;
};

class Md2Triangle
{
	public:
		short vertex[3];
		short texture[3];
};

class Md2Frame
{
	public:
		float scale[3];
		float translate[3];
		char name[16]; //40 bytes to here.
		Md2ReadTriangleStruct *vertices;
};

#endif