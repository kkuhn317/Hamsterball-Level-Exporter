// Console.cpp: implementation of the Console class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Console.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Console gOut;

Console::Console()
{
#ifndef FILE

	mHwnd=NULL;

	mHwnd=(long)FindWindow(NULL,"C++ Console");
	if (mHwnd==NULL) {}//MessageBox(0,"The C++ Console was not found.  Console reporting will be lost.","No C++ Console!",MB_OK);
	else 
	{
		COPYDATASTRUCT copy;
		copy.cbData=0;
		copy.lpData=NULL;
		SendMessage((HWND)mHwnd,WM_COPYDATA,NULL,(long)&copy);

		Out("*** BEGIN SESSION ***");
	}

#else
	mHwnd=creat("DEBUG.TXT",S_IREAD|S_IWRITE);close(mHwnd);
	Out("*** BEGIN SESSION ***");
#endif
}

Console::~Console()
{
	Out("*** END SESSION NORMALLY ***");
}

void Console::Out(char *format, ...)
{
#ifndef FILE
	if (mHwnd==NULL) return;

	char *xstring=(char*)GlobalAlloc(GMEM_FIXED,2048);

	va_list argp;
 	va_start(argp, format);
	vsprintf(xstring,format, argp);
	va_end(argp);

	COPYDATASTRUCT copy;
	copy.cbData=strlen(xstring)+1;
	copy.lpData=xstring;

	SendMessage((HWND)mHwnd,WM_COPYDATA,NULL,(long)&copy);

	GlobalFree(xstring);
#else
	mHwnd=open("DEBUG.TXT",O_BINARY|O_RDWR);
	lseek(mHwnd,0,SEEK_END);

	char *xstring=(char*)GlobalAlloc(GMEM_FIXED,2048);

	va_list argp;
 	va_start(argp, format);
	vsprintf(xstring,format, argp);
	va_end(argp);

	char crlf[2]={13,10};

	write(mHwnd,xstring,strlen(xstring));
	write(mHwnd,crlf,2);
	close(mHwnd);
#endif
}

