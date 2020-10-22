/**********************************************************************
 *<
	FILE: MeshExport.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __MESHEXPORT__H
#define __MESHEXPORT__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include "meshstripper.h"



extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#define MESHEXPORT_CLASS_ID	Class_ID(0x76397efe, 0x6e1edd6c)

class MeshExport : public SceneExport {
	public:


		static HWND hParams;


		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL SupportsOptions(int ext, DWORD options);
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);


		
		//Constructor/Destructor

		MeshExport();
		~MeshExport();		

		void		WriteString(char *string);
		void		WriteInt(int theInt);
		void		WriteFloat(float theFloat);
		void		WriteMemory(void *theMemory, int theSize);
		void		Seek(int aSpot);
		void		OpenFile(TCHAR *filename);
		void		CloseFile();


	public:
		int			mFile;
		int			mResult;

		int			mRenderableObjects;
		Cube		mCube;


};


class MeshExportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new MeshExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return MESHEXPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("MeshExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};


#endif // __MESHEXPORT__H
