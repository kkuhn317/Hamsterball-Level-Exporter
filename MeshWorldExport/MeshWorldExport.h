/**********************************************************************
 *<
	FILE: MeshWorldExport.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __MESHWORLDEXPORT__H
#define __MESHWORLDEXPORT__H

#include "stdafx.h"
#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "console.h"


extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#define MESHWORLDEXPORT_CLASS_ID	Class_ID(0x447a4ed8, 0x3eaff0e2)

class WorldMesh;

class MeshWorldExport : public SceneExport {
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

		MeshWorldExport();
		~MeshWorldExport();	

		void		WriteString(char *string);
		void		WriteInt(int theInt);
		void		WriteFloat(float theFloat);
		void		WriteMemory(void *theMemory, int theSize);
		void		Seek(int aSpot);
		void		OpenFile(TCHAR *filename);
		void		CloseFile();

public:
		bool		mOkay;
		int			mCubeSize;

		int			mFile;
		int			mResult;

		Cube		mCube;
		int			mRenderableObjects;

		WorldMesh	*mMesh;

		PointerList	mLights;	// Lights in the scene

		PointerList mPoints;	// Points in the scene (like, to position something)

		PointerList mSplines;	// Splines in the scenes (lists of vertices)

};


class MeshWorldExportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new MeshWorldExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return MESHWORLDEXPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("MeshWorldExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



#endif // __MESHWORLDEXPORT__H
