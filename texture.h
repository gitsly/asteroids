#ifndef __Texture_H__
#define __Texture_H__

template <class T> class CLink;

class CTexture
{
	public:
		CTexture();
		CTexture(char *f);

		char *file;		
		unsigned int glStorage; // Points to the storage point in the GL system.

		void Load(char *f);
};

// Texture handler.
class CTextureBase
{
	public:	
		CTextureBase();
		~CTextureBase();

		CTexture *GetTexture(char *name);
		void ReloadTextures();

		CLink<CTexture> *tbase;	 // Linked list of textures.
};

#endif