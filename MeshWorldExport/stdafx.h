// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A751B189_5623_40DF_A838_D0C8211C0DB4__INCLUDED_)
#define AFX_STDAFX_H__A751B189_5623_40DF_A838_D0C8211C0DB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdarg.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <memory.h>
#include <dos.h>
#include <math.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <malloc.h>
#include <float.h>
#include <windows.h>
#include <mmsystem.h>
#include <process.h>
#include <assert.h>
#include <commctrl.h>
#include <commdlg.h>
#include <objbase.h>

#include <d3d8.h>			
#include <d3dx8math.h>			
#include <d3dx8tex.h>
#include <d3d8types.h>
#include <D3dx8core.h>
#include <dinput.h>
#include <Dsound.h>

#define safedelete(m) {if (m!=NULL) {delete m;m=NULL;}}
#define safedeletelist(m) {if (m!=NULL) {delete [] m;m=NULL;}}
#define safefree(m) {if (m!=NULL) {free(m);m=NULL;}};
#define saferelease(m) {if (m!=NULL) {m->Release();m=NULL;}}

#define WM_MOUSEWHEEL                   0x020A

#include "Math.h"
#include "PointerList.h"

extern Math				gMath;




// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A751B189_5623_40DF_A838_D0C8211C0DB4__INCLUDED_)
