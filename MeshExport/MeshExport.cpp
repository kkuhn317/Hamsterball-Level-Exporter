/**********************************************************************
 *<
	FILE: MeshExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/
#include "Stdafx.h"

#include "MeshExport.h"
#include "NodeEnumerator.h"










/*

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
		void		OpenFile(TCHAR *filename);
		void		CloseFile();


	public:
		int			mFile;


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
*/



static MeshExportClassDesc MeshExportDesc;
ClassDesc2* GetMeshExportDesc() { return &MeshExportDesc; }


BOOL CALLBACK MeshExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static MeshExport *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (MeshExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}


//--- MeshExport -------------------------------------------------------
MeshExport::MeshExport()
{

}

MeshExport::~MeshExport() 
{

}

int MeshExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *MeshExport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("Mesh");
}

const TCHAR *MeshExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Greekfire Mesh");
}
	
const TCHAR *MeshExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("Mesh");
}

const TCHAR *MeshExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("John Raptis");
}

const TCHAR *MeshExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *MeshExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *MeshExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int MeshExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void MeshExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL MeshExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

void MeshExport::WriteString(char *string)
{
	int aLength=strlen(string)+1;
	write(mFile,&aLength,sizeof(aLength));
	write(mFile,string,aLength);
}

void MeshExport::WriteInt(int theInt)
{
	write(mFile,&theInt,sizeof(int));
}

void MeshExport::WriteFloat(float theFloat)
{
	write(mFile,&theFloat,sizeof(float));
}

void MeshExport::WriteMemory(void *theMemory, int theSize)
{
	write(mFile,theMemory,theSize);
}

void MeshExport::OpenFile(TCHAR *filename)
{
	mFile=creat(filename,S_IREAD|S_IWRITE);close(mFile);
	mFile=open(filename,O_BINARY|O_RDWR);

}

void MeshExport::CloseFile()
{
	close(mFile);
}

void MeshExport::Seek(int aSpot)
{
	lseek(mFile,0,SEEK_SET);
}




int	MeshExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly

	//MessageBox(0,name,"The Filename",MB_OK);

	mResult=true;
	mRenderableObjects=0;


	NodeEnumerator aNode;
	aNode.mExporter=this;

	OpenFile((TCHAR*)name);
	WriteInt(mRenderableObjects);

	mCube.Reset();

	ei->theScene->EnumTree(&aNode);

	//
	// Write out the bounding box...
	//
	WriteMemory(&mCube,sizeof(mCube));

	//
	// Go to the beginning and write out how many objects
	// we're dealing with here...
	//
	Seek(0);
	WriteInt(mRenderableObjects);
	CloseFile();

	// Well, we need to go through the interface and see about our objects.
	// Let's start by making the name of our objects appear.

/*

	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				MeshExportOptionsDlgProc, (LPARAM)this);
*/

	return mResult;
}


