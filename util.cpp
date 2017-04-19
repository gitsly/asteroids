/* Filename:	util.cpp
 * Author:		 Martin Collberg
 * Email:		  comayog@swipnet.se
 * Created:		00-10-05
 * Modified:   00-10-09
 *
 * Misc functions (utilities)
 * 
 */

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "engine.h"
#include "util.h"

void end(char *fmt, ...)
{
	va_list     argptr;
	char        txt[256];

	va_start (argptr,fmt); // Start list after variable fmt.  va_arg(argptr, double)  get list argument into a variable in this case (double).
	vsprintf (txt,fmt,argptr);
	va_end   (argptr);


	MessageBox(NULL, txt, "Exit message", MB_OK); 

	Destroy ( engine );
	KillGLWindow();
	exit(0);
}
