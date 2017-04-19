#ifndef __Object3d_Included__
#define __Object3d_Included__

#include "vector.h"

class CTexture;
class C3dObject;


class CTextureCoord
{
	public:
		float u, v;
};

class CVertex : public Vector3
{
	public:
		CVertex();
		CVertex(float x1, float y1, float z1);
		Vector3 n;

		void Set(float x1, float y1, float z1);
};

class CFace
{
	public:
		CFace();
		~CFace();

		int v[3];	
		int t[3]; // needed for that stupid md2..	
		CTexture *texture;
};

class CFrame
{
	public:
		CFrame();
		~CFrame();

		unsigned int displayList; //Pointer to compiled frame (in gl engine).
		CVertex *vertex;
		Vector3 center;

		Vector3 CalculateCenterPoint( int numVertices );
};

class C3dObject
{
	public:
		C3dObject ();
		C3dObject (int numFaces1, int numVertices1 ,int numFrames1);
		~C3dObject ();
		CFace *face;
		CFrame *frame;
		CTextureCoord *textureCoord;

		char name[64];
		int numFaces;
		int numFrames;
		int numVertices;
		int numTextureCoords;

		void LoadMd2(char *name, char *skinfile, float scale);
		void Load3ds(char *file, float scale); 
		void Draw(int frm);
		void CalcSimpleNormals();
		void MakeFacetNormal (int facenum, int frm, float *fn);
		void GenerateLightNormals ();
		void InverseLightNormals();
		void GenerateDisplayList();

};



#endif