#ifndef __Util_H__
#define __Util_H__

template <class T>
void Destroy(T *ptr)
{
	if(ptr) 
		delete ptr;
	ptr = NULL;
}

template <class T>
void DestroyArray(T *ptr)
{
	if(ptr) 
		delete[] ptr;
	ptr = NULL;
}

void end(char *fmt, ...);

#endif
