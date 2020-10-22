// Console.h: interface for the Console class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSOLE_H__08398910_4C1F_4791_9F37_76F6D8301CFB__INCLUDED_)
#define AFX_CONSOLE_H__08398910_4C1F_4791_9F37_76F6D8301CFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define FILE

class Console  
{
public:
	Console();
	virtual ~Console();


	void					Out(char *format, ...);

private:
	long					mHwnd;
};



#endif // !defined(AFX_CONSOLE_H__08398910_4C1F_4791_9F37_76F6D8301CFB__INCLUDED_)
