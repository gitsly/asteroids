#ifndef __bitmap_h__
#define __bitmap_h__

struct Rgb
{
	unsigned char r, g, b;
} ;

class Bitmap
{
	public:
		Bitmap () : data(NULL), w(0), h(0)  {}
		~Bitmap();
		Rgb *data;
		int w, h;

		bool LoadBitmap(char *file); // Selects sub-load function depending on file extension.
		bool LoadBmp(char *file);
		bool LoadPcx(char *file);
		void Stretch(int nw, int nh);
		int operator !() { 	return (data) ? true : false; }
};

typedef Rgb palette[256];

#endif