// WorldMesh.cpp: implementation of the WorldMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WorldMesh.h"
#include "MeshWorldExport.h"

#include "console.h"
extern Console gOut;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WorldOptimizeKit::WorldOptimizeKit()
{
	mVertexList=NULL;
	mVertexCount=0;
}

WorldOptimizeKit::~WorldOptimizeKit()
{
	safefree(mVertexList);
}

int WorldOptimizeKit::AddVertexList(Vertex *theList, int theCount)
{
	if (mVertexList==NULL) mVertexList=(Vertex*)malloc(sizeof(Vertex)*theCount);
	else mVertexList=(Vertex*)realloc(mVertexList,sizeof(Vertex)*(theCount+mVertexCount));

	Vertex *theDest=mVertexList;
	theDest+=mVertexCount;

	memcpy(theDest,theList,sizeof(Vertex)*theCount);

	int aAddedPosition=mVertexCount;
	mVertexCount+=theCount;

	return aAddedPosition;
}


WorldMesh::WorldMesh() : xMesh()
{
	mSubMeshes.Clear();
	mSubdivided=false;


}

WorldMesh::WorldMesh(char *theFilename) : xMesh(theFilename)
{
	mSubMeshes.Clear();
	mSubdivided=false;
}


WorldMesh::~WorldMesh()
{
	for (xMesh *aSubMesh=(xMesh*)mSubMeshes.First();aSubMesh!=NULL;aSubMesh=(xMesh*)mSubMeshes.Next()) delete aSubMesh;
	mSubMeshes.Clear();
}

void WorldMesh::Generate(int xCount, int yCount, int zCount, bool shouldOptimize)
{
	mSubMeshes.Clear();

	float aSizeX=mMeshData->mCube.mCorner2.mX-mMeshData->mCube.mCorner1.mX;
	float aSizeY=mMeshData->mCube.mCorner2.mY-mMeshData->mCube.mCorner1.mY;
	float aSizeZ=mMeshData->mCube.mCorner2.mZ-mMeshData->mCube.mCorner1.mZ;

	aSizeX/=(float)xCount;
	aSizeY/=(float)yCount;
	aSizeZ/=(float)zCount;


	for (int aSpanX=0;aSpanX<xCount;aSpanX++)
	{
		for (int aSpanY=0;aSpanY<yCount;aSpanY++)
		{
			for (int aSpanZ=0;aSpanZ<zCount;aSpanZ++)
			{
				Cube aCube;
				aCube.mCorner1.mX=mMeshData->mCube.mCorner1.mX+(aSizeX*(float)aSpanX);
				aCube.mCorner1.mY=mMeshData->mCube.mCorner1.mY+(aSizeY*(float)aSpanY);
				aCube.mCorner1.mZ=mMeshData->mCube.mCorner1.mZ+(aSizeZ*(float)aSpanZ);

				aCube.mCorner2.mX=aCube.mCorner1.mX+aSizeX;
				aCube.mCorner2.mY=aCube.mCorner1.mY+aSizeY;
				aCube.mCorner2.mZ=aCube.mCorner1.mZ+aSizeZ;
				WorldMesh *aAreaMesh=new WorldMesh();

				if (aAreaMesh->MakeSubMesh(this,&aCube)==true) 
				{
					mSubMeshes.Add(aAreaMesh);

					//if (shouldOptimize==true) aAreaMesh->mMeshData->Optimize();

					//memcpy(&aAreaMesh->mMeshData->mCube,&aCube,sizeof(aCube));
				}
				else 
				{
					delete aAreaMesh;
				}
			}
		}
	}

//	mSizeX=aSizeX;
//	mSizeY=aSizeY;
//	mSizeZ=aSizeZ;

	mSizeX=mMeshData->mCube.mCorner2.mX-mMeshData->mCube.mCorner1.mX;
	mSizeY=mMeshData->mCube.mCorner2.mY-mMeshData->mCube.mCorner1.mY;
	mSizeZ=mMeshData->mCube.mCorner2.mZ-mMeshData->mCube.mCorner1.mZ;


	mSubdivided=false;
	if (mSubMeshes.GetCount()>0) mSubdivided=true;
}

bool WorldMesh::MakeSubMesh(WorldMesh *sourceMesh, Cube *theCube, bool preserveSource)
{
	return xMesh::MakeSubMesh(sourceMesh,theCube,preserveSource);
}


void WorldMesh::SliceGenerate(int theStep, bool shouldOptimize)
{
	mSubMeshes.Clear();

	int aLowestExtent=mMeshData->mCube.mCorner1.mX;
	aLowestExtent=min(aLowestExtent,mMeshData->mCube.mCorner1.mY);
	aLowestExtent=min(aLowestExtent,mMeshData->mCube.mCorner1.mZ);

	aLowestExtent/=theStep;
	aLowestExtent*=theStep;
	aLowestExtent-=theStep;

	int aHighestExtent=mMeshData->mCube.mCorner2.mX;
	aHighestExtent=max(aHighestExtent,mMeshData->mCube.mCorner2.mY);
	aHighestExtent=max(aHighestExtent,mMeshData->mCube.mCorner2.mZ);

	aHighestExtent/=theStep;
	aHighestExtent*=theStep;
	aHighestExtent+=theStep;


	int theFrom=aLowestExtent;
	int theTo=aHighestExtent;

	for (int aSpanX=theFrom+theStep;aSpanX<theTo+theStep;aSpanX+=theStep)
	{
		for (int aSpanY=theFrom+theStep;aSpanY<=theTo+theStep;aSpanY+=theStep)
		{
			for (int aSpanZ=theFrom+theStep;aSpanZ<=theTo+theStep;aSpanZ+=theStep)
			{
				Cube aCube;
				aCube.mCorner1.mX=theFrom;
				aCube.mCorner1.mY=theFrom;
				aCube.mCorner1.mZ=theFrom;
				aCube.mCorner2.mX=aSpanX-1;
				aCube.mCorner2.mY=aSpanY-1;
				aCube.mCorner2.mZ=aSpanZ-1;

				WorldMesh *aAreaMesh=new WorldMesh();
				if (aAreaMesh->MakeSubMesh(this,&aCube)==true) 
				{
					mSubMeshes.Add(aAreaMesh);
				}
				else 
				{
					delete aAreaMesh;
				}
			}
		}
	}

	mSubdivided=false;
	if (mSubMeshes.GetCount()>0) mSubdivided=true;
}

void WorldMesh::WorldOptimizeDump(WorldOptimizeKit *theKit)
{
	if (!mSubdivided) 
	{
		//
		// Add our Meshdata's vertexlist to the kit,
		// and adjust all the triangle strips to adjust
		// to the new reference.
		//

		int aPosition=theKit->AddVertexList(mMeshData->mVertexList,mMeshData->mVertexCount);

		EnumPointerlist(GeometryObject,aGeom,mMeshData->mGeoms)
		{
			EnumPointerlist(TriangleStrip,aStrip,aGeom->mStrips)
			{
				aStrip->mSecondaryVertexListReference=aStrip->mVertexListReference+aPosition;
			}
		}
	}
	else
	{
		EnumPointerlist(WorldMesh,aWorldMesh,mSubMeshes) aWorldMesh->WorldOptimizeDump(theKit);
	}
}

void WorldMesh::WorldOptimize()
{
	////////////////////////////////////////////////////////
	//
	// This attempts to move all children of the worldmesh
	// into a single vertex buffer.  
	//
	// Only works on a fully optimized world... just takes
	// all the vertex buffers of all the children,
	// appends them together, and adjusts the vertexref
	// in all the triangle strips, and sets a bit to
	// intercept draw commands into our private routine.
	//
	////////////////////////////////////////////////////////

	//
	// First, make sure we're all optimized
	//

	//
	// Then, make sure we can make a vertex buffer...
	//

	//
	// Dump all our submeshes into the vertex buffer...
	//

	WorldOptimizeDump(&mKit);

/*

	void *aVertices;
	mVertexBuffer->Lock(0,0,(unsigned char**)&aVertices,0);
	memcpy(aVertices,aKit.mVertexList,(aKit.mVertexCount)*sizeof(Vertex));
	mVertexBuffer->Unlock();
*/
}

void WorldMesh::GetSubMeshes(PointerList *aResultList)
{
	if (mSubdivided)
	{
		EnumPointerlist(WorldMesh,aMesh,mSubMeshes) aMesh->GetSubMeshes(aResultList);
		return;
	}

	aResultList->Add(this);
}

void WorldMesh::Dump(MeshWorldExport *mExporter)
{
	mExporter->WriteMemory(&mMeshData->mCube,sizeof(mMeshData->mCube));			// Write octree cube area
/*
	gOut.Out("CUBE: %f %f %f - %f %f %f",
		mMeshData->mCube.mCorner1.mX,
		mMeshData->mCube.mCorner1.mY,
		mMeshData->mCube.mCorner1.mZ,
		mMeshData->mCube.mCorner2.mX,
		mMeshData->mCube.mCorner2.mY,
		mMeshData->mCube.mCorner2.mZ);
*/


	if (mSubdivided)
	{
		mExporter->WriteInt(mSubMeshes.GetCount());								// Write # of submeshes
		EnumPointerlist(WorldMesh,aMesh,mSubMeshes) aMesh->Dump(mExporter);		// Dump the submeshes
	}
	else
	{
		mExporter->WriteInt(0);								// Write 0 submeshes, as a flag
		mExporter->WriteInt(mMeshData->mGeoms.GetCount());	// Write # of geoms

		EnumPointerlist(GeometryObject,aGeom,mMeshData->mGeoms)
		{
			if (aGeom->mName==NULL) mExporter->WriteString("");
			else 	
			{
				//
				// Only write out the name of the geom if it's a NOTIFY_ geom
				//
				if (aGeom->mName[1]==':' || strstr(aGeom->mName,"NOCOLLIDE")) mExporter->WriteString(aGeom->mName);
/*
				if (strnicmp(aGeom->mName,"N:",strlen("N:"))==0 ||
					strnicmp(aGeom->mName,"T:",strlen("T:"))==0 ||
					strnicmp(aGeom->mName,"E:",strlen("E:"))==0
					) mExporter->WriteString(aGeom->mName);
*/
				else mExporter->WriteString("");
			}

			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Ambient.r);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Ambient.g);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Ambient.b);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Ambient.a);

			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Diffuse.r);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Diffuse.g);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Diffuse.b);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Diffuse.a);

			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Specular.r);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Specular.g);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Specular.b);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Specular.a);

			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Emissive.r);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Emissive.g);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Emissive.b);
			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Emissive.a);

			mExporter->WriteFloat(aGeom->mMaterialRef.mMaterial.Power);
			mExporter->WriteInt(aGeom->mMaterialRef.mHasReflection);

			if (aGeom->mMaterialRef.mTexture==NULL) mExporter->WriteInt(0);
			else
			{
				mExporter->WriteInt(1);
				mExporter->WriteString(aGeom->mMaterialRef.mTexture);
			}
			mExporter->WriteInt(aGeom->mStrips.GetCount());

			EnumPointerlist(TriangleStrip,aStrip,aGeom->mStrips)
			{
				mExporter->WriteInt(aStrip->mTriangles);
				mExporter->WriteInt(aStrip->mSecondaryVertexListReference);
			}
		}
	}
}



