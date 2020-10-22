/**********************************************************************
 *<
	FILE: MeshWorldExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "MeshWorldExport.h"
#include "Mesh.h"
#include "WorldMesh.h"
#include "nodeenumerator.h"
#include "slicenodeenumerator.h"
#include "light.h"


#include "console.h"

extern Console gOut;

static MeshWorldExportClassDesc MeshWorldExportDesc;
ClassDesc2* GetMeshWorldExportDesc() { return &MeshWorldExportDesc; }


BOOL CALLBACK MeshWorldExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static MeshWorldExport *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (MeshWorldExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));

			SetDlgItemInt(hWnd,IDC_CUBESIZE,2000,false);

			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			imp->mOkay=false;
			return TRUE;

		case WM_COMMAND:
			{
				switch (LOWORD(wParam))
				{
				case IDOK:
					{
						imp->mCubeSize=GetDlgItemInt(hWnd,IDC_CUBESIZE,FALSE,FALSE);
						imp->mOkay=true;
						EndDialog(hWnd, 1);
						return TRUE;
						break;
					}
				}
				break;
			}
	}
	return FALSE;
}


//--- MeshWorldExport -------------------------------------------------------
MeshWorldExport::MeshWorldExport()
{
	mRenderableObjects=0;

}

MeshWorldExport::~MeshWorldExport() 
{

}

int MeshWorldExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *MeshWorldExport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("MeshWorld");
}

const TCHAR *MeshWorldExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("GreekFire World Mesh");
}
	
const TCHAR *MeshWorldExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("World Mesh");
}

const TCHAR *MeshWorldExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("John Raptis");
}

const TCHAR *MeshWorldExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *MeshWorldExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *MeshWorldExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int MeshWorldExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void MeshWorldExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL MeshWorldExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

void MeshWorldExport::WriteString(char *string)
{
	int aLength=strlen(string)+1;
	write(mFile,&aLength,sizeof(aLength));
	write(mFile,string,aLength);
}

void MeshWorldExport::WriteInt(int theInt)
{
	write(mFile,&theInt,sizeof(int));
}

void MeshWorldExport::WriteFloat(float theFloat)
{
	write(mFile,&theFloat,sizeof(float));
}

void MeshWorldExport::WriteMemory(void *theMemory, int theSize)
{
	write(mFile,theMemory,theSize);
}

void MeshWorldExport::OpenFile(TCHAR *filename)
{
	mFile=creat(filename,S_IREAD|S_IWRITE);close(mFile);
	mFile=open(filename,O_BINARY|O_RDWR);

}

void MeshWorldExport::CloseFile()
{
	close(mFile);
}

void MeshWorldExport::Seek(int aSpot)
{
	lseek(mFile,0,SEEK_SET);
}

int	MeshWorldExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly

	// Remember, when you create a mesh, it has no meshdata...
	// So you need to make one of those...
	// Then add geoms to it, plus triangles, until it's full.

	Point3 aBkgColor=i->GetBackGround(0,FOREVER);
	Point3 aAmbientColor=i->GetAmbient(0,FOREVER);

	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				MeshWorldExportOptionsDlgProc, (LPARAM)this);

	if (mOkay==true)
	{
		// We need to slice our mesh!
		// In all the right places... cut in two, then cut in two,
		// then cut in two, etc, etc, until we're down to the size of 
		// our requested 'cube size.'

RELOOP:

		mMesh=new WorldMesh();
		mMesh->mMeshData=new MeshData();

		NodeEnumerator aNode;
		aNode.mExporter=this;

		ei->theScene->EnumTree(&aNode);

		PointerList xGenList;
		mMesh->GetSubMeshes(&xGenList);

		if (mCubeSize>0)
		{
			mMesh->Generate(2,2,2,false);
		}
		mMesh->GetSubMeshes(&xGenList);

		float aSizeX=mMesh->mSizeX;
		float aSizeY=mMesh->mSizeY;
		float aSizeZ=mMesh->mSizeZ;

		int aSubCount;
		aSubCount=0;

		bool aBadBreak=false;

		while ((aSizeX>mCubeSize || aSizeY>mCubeSize || aSizeZ>mCubeSize) && mCubeSize!=0)
		{
//			gOut.Out("  Current Break: %f %f %f (%d)",aSizeX,aSizeY,aSizeZ,aSubCount);

			aSubCount++;
			PointerList aGenList;

			mMesh->GetSubMeshes(&aGenList);

			float aOldSizeX=aSizeX;
			float aOldSizeY=aSizeY;
			float aOldSizeZ=aSizeZ;

			EnumPointerlist(WorldMesh,aMesh,aGenList)
			{
				aMesh->Generate(2,2,2,false);

				aSizeX=min(aSizeX,aMesh->mSizeX);
				aSizeY=min(aSizeY,aMesh->mSizeY);
				aSizeZ=min(aSizeZ,aMesh->mSizeZ);
			}
			aGenList.Clear();

			if (aSizeX==aOldSizeX && aSizeY==aOldSizeY && aSizeZ==aOldSizeZ) 
			{
				// Problem...
				// We CAN'T break it down to the target.
				// So, what we're going to do is re-adjust what the
				// target is, and use THAT.

//				gOut.Out("**** BAD BREAK ***");
				aBadBreak=true;

				break;
			}
		}

		if (aBadBreak==true)
		{
			mCubeSize=aSizeX;
			mCubeSize=max(mCubeSize,aSizeY);
			mCubeSize=max(mCubeSize,aSizeZ);
			mCubeSize+=10;

			delete mMesh;
			mMesh=NULL;

			goto RELOOP;
		}

//		gOut.Out("Broken down to: %f %f %f (%d)",aSizeX,aSizeY,aSizeZ,aSubCount);

		PointerList aGenList;
		mMesh->GetSubMeshes(&aGenList);

		EnumPointerlist(WorldMesh,aMesh,aGenList) 
		{
			aMesh->mMeshData->Optimize();
		}
		aGenList.Clear();

		mMesh->WorldOptimize();

		OpenFile((char*)name);

		//
		// Write out the ref points in the scene...
		//
		WriteInt(mPoints.GetCount());
		EnumPointerlist(xPoint,aPoint,mPoints)
		{
			WriteString(aPoint->mName);
			WriteFloat(aPoint->mPosition.x);
			WriteFloat(aPoint->mPosition.z);
			WriteFloat(aPoint->mPosition.y);
			WriteFloat(aPoint->mRotation.x);
			WriteFloat(aPoint->mRotation.z);
			WriteFloat(aPoint->mRotation.y);

			WriteInt(aPoint->mHasMaterial);

			if (aPoint->mHasMaterial)
			{
				//FUNK

				WriteFloat(aPoint->mMaterial.mMaterial.Ambient.r);
				WriteFloat(aPoint->mMaterial.mMaterial.Ambient.g);
				WriteFloat(aPoint->mMaterial.mMaterial.Ambient.b);
				WriteFloat(aPoint->mMaterial.mMaterial.Ambient.a);

				WriteFloat(aPoint->mMaterial.mMaterial.Diffuse.r);
				WriteFloat(aPoint->mMaterial.mMaterial.Diffuse.g);
				WriteFloat(aPoint->mMaterial.mMaterial.Diffuse.b);
				WriteFloat(aPoint->mMaterial.mMaterial.Diffuse.a);

				WriteFloat(aPoint->mMaterial.mMaterial.Specular.r);
				WriteFloat(aPoint->mMaterial.mMaterial.Specular.g);
				WriteFloat(aPoint->mMaterial.mMaterial.Specular.b);
				WriteFloat(aPoint->mMaterial.mMaterial.Specular.a);

				WriteFloat(aPoint->mMaterial.mMaterial.Emissive.r);
				WriteFloat(aPoint->mMaterial.mMaterial.Emissive.g);
				WriteFloat(aPoint->mMaterial.mMaterial.Emissive.b);
				WriteFloat(aPoint->mMaterial.mMaterial.Emissive.a);

				WriteFloat(aPoint->mMaterial.mMaterial.Power);
				WriteInt(aPoint->mMaterial.mHasReflection);

				if (aPoint->mMaterial.mTexture==NULL) WriteInt(0);
				else
				{
					WriteInt(1);
					WriteString(aPoint->mMaterial.mTexture);
				}
			}
		}

		//
		// Write out the splines...
		//
		WriteInt(mSplines.GetCount());
		EnumPointerlist(xSpline,aSpline,mSplines)
		{
			WriteString(aSpline->mName);
			WriteInt(aSpline->mPointList.GetCount());
			EnumPointerlist(Point3,aPt,aSpline->mPointList)
			{
				WriteFloat(aPt->x);
				WriteFloat(aPt->z);
				WriteFloat(aPt->y);
			}
		}


		//
		// Write out the lights...
		//
		WriteInt(mLights.GetCount());
		EnumPointerlist(xLight,aLight,mLights)
		{
			WriteInt(aLight->mType);
			switch (aLight->mType)
			{
			case xLight::DISTANTLIGHT:
				{
					WriteFloat(aLight->mPosition.x);
					WriteFloat(aLight->mPosition.z);
					WriteFloat(aLight->mPosition.y);

					WriteFloat(aLight->mLookat.x);
					WriteFloat(aLight->mLookat.z);
					WriteFloat(aLight->mLookat.y);

					WriteFloat(aLight->mColor.x);
					WriteFloat(aLight->mColor.y);
					WriteFloat(aLight->mColor.z);
					break;
				}
			}

			delete aLight;
		}
		mLights.Clear();

		WriteFloat(aBkgColor.x);
		WriteFloat(aBkgColor.y);
		WriteFloat(aBkgColor.z);

		WriteFloat(aAmbientColor.x);
		WriteFloat(aAmbientColor.y);
		WriteFloat(aAmbientColor.z);

		// Write all the vertices...
		// Now write data about meshes/submeshes... all the geoms and cubes and shit...

		// Okay, you write:

		WriteInt(mMesh->mKit.mVertexCount);
		WriteMemory(mMesh->mKit.mVertexList,mMesh->mKit.mVertexCount*sizeof(Vertex));

		mMesh->Dump(this);

		// Mesh
		//  Cube
		//  Subdivided
		//
		//	 Mesh
		//    Cube
		//    Subdivided
		//    Geom/Strips
		//
		//	 Mesh
		//    Cube
		//    Subdivided
		//    Geom/Strips

		CloseFile();




/*
		SliceNodeEnumerator aSliceNode;
		aSliceNode.mSlicePoint.x=(mMesh->mMeshData->mCube.mCorner1.mX+mMesh->mMeshData->mCube.mCorner2.mX)/2;
		aSliceNode.mSlicePoint.z=(mMesh->mMeshData->mCube.mCorner1.mY+mMesh->mMeshData->mCube.mCorner2.mY)/2;
		aSliceNode.mSlicePoint.y=(mMesh->mMeshData->mCube.mCorner1.mZ+mMesh->mMeshData->mCube.mCorner2.mZ)/2;
		ei->theScene->EnumTree(&aSliceNode);
*/


		// Okay, now...
		// Using the extents of mMesh->mMeshData->mCube...
		// we slice it in half on all axis.

		// We keep doing that until the results are smaller than the requested cube size, right?
		// Then we re-dump it all into the mesh, and cut the mesh up by the edges.

		// Okay!  We generate submeshes until such a time as the smallest submesh's 
		// size is <= than the cube size we put in.

		// Then dump it.  And dump the light too.

	}

	return TRUE;
}


