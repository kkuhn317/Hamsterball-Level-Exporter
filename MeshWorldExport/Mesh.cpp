// Mesh.cpp: implementation of the Mesh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mesh.h"
#include "MeshStripper.h"

#include "console.h"

extern Console gOut;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MeshData::MeshData()
{
	mMaterials=NULL;
	mVertexList=NULL;
	mVertexCount=0;
	mOwnResources=true;
	mOptimized=false;

	mCube.mCorner1.mX=9999999;
	mCube.mCorner1.mY=9999999;
	mCube.mCorner1.mZ=9999999;
	mCube.mCorner2.mX=-9999999;
	mCube.mCorner2.mY=-9999999;
	mCube.mCorner2.mZ=-9999999;

}
MeshData::~MeshData()
{
	if (mOwnResources==true)
	{
		if (mMaterials!=NULL)
		{
			for (int aCount=0;aCount<mMaterialCount;aCount++)
			{
				if (mMaterials[aCount].mTexture!=NULL)
				{
					delete [] mMaterials[aCount].mTexture;
				}
			}
		}
	}

	if (mMaterials!=NULL)
	{
		delete [] mMaterials;
		mMaterials=NULL;
	}

	for (GeometryObject *aGeom=(GeometryObject*)mGeoms.First();aGeom!=NULL;aGeom=(GeometryObject*)mGeoms.Next())
	{
		delete aGeom;
	}
	mGeoms.Clear();
}

void MeshData::Optimize()
{
	OptimizeRoutine();
	for (GeometryObject *aGeom=(GeometryObject*)mGeoms.First();aGeom!=NULL;aGeom=(GeometryObject*)mGeoms.Next())
	{
		if (aGeom->mOptimized==true)
		{
			aGeom->mFaces.Clear();
		}
	}
}

void MeshData::OptimizeRoutine()
{
	for (GeometryObject *aGeom=(GeometryObject*)mGeoms.First();aGeom!=NULL;aGeom=(GeometryObject*)mGeoms.Next())
	{
		MeshStripper *aStripper=new MeshStripper(aGeom->mFaces.GetCount());

		for (Triangle *aT=(Triangle*)aGeom->mFaces.First();aT!=NULL;aT=(Triangle*)aGeom->mFaces.Next())
		{
			aStripper->AddVertex(&aT->mVertex[0]);
			aStripper->AddVertex(&aT->mVertex[1]);
			aStripper->AddVertex(&aT->mVertex[2]);
		}

		for (Triangle *aT=(Triangle*)aGeom->mFaces.First();aT!=NULL;aT=(Triangle*)aGeom->mFaces.Next())
		{
			aStripper->AddTriangle(aT);
		}

		if (aStripper->Go()!=-1)
		{

			aGeom->mOptimized=true;
			aStripper->GetResults(&aGeom->mStrips);
		}
		else
		{
			char aOptMessage[256];
			sprintf(aOptMessage,"Optimize requested, but failed!\n\n%d faces",aGeom->mFaces.GetCount());

			MessageBox(0,aOptMessage,"Mesh",MB_OK);
			aGeom->mOptimized=false;
			aStripper->Clear();
		}

		delete aStripper;
	}
	SetupVertexBuffer();

}

void MeshData::SetupVertexBuffer()
{
	mOptimized=true;
	EnumPointerlist(GeometryObject,aGeom,mGeoms)
	{
		if (aGeom->mOptimized==false) mOptimized=false;
	}

	if (mOptimized==true) 
	{
		mVertexCount=0;
		EnumPointerlist(GeometryObject,aGeom,mGeoms) 
		{
			EnumPointerlist(TriangleStrip,aStrip,aGeom->mStrips) 
			{
				mVertexCount+=aStrip->mTriangles+2;
			}
		}
		mVertexList=new Vertex[mVertexCount];

		//
		// Clone all the Geoms and Strips.
		// The Strips don't need to clone their vertices-- they're getting
		// dumped into the main pool of vertices
		//
		int aVertexRef=0;
		EnumPointerlist(GeometryObject,aGeom,mGeoms) 
		{
			EnumPointerlist(TriangleStrip,aStrip,aGeom->mStrips) 
			{
				aStrip->mVertexListReference=aVertexRef;
				memcpy(&mVertexList[aVertexRef],aStrip->mVertices,(aStrip->mTriangles+2)*sizeof(Vertex));
				aVertexRef+=aStrip->mTriangles+2;

				safedelete(aStrip->mVertices);
			}
		}
		//MakeVertexBuffer();
	}
}



xMesh::xMesh()
{
	mImported=false;
	mMeshData=NULL;

	mOwnMeshData=false;
}

xMesh::xMesh(MeshData *theMeshData)
{
	mImported=false;
	mMeshData=NULL;


	mMeshData=theMeshData;
	mOwnMeshData=false;
}


xMesh::xMesh(char *theFilename)
{
	mImported=false;
	mMeshData=NULL;

	mOwnMeshData=false;
}


xMesh::~xMesh()
{
	if (mOwnMeshData)
	{
		delete mMeshData;
		mMeshData=NULL;
	}


}




bool xMesh::MakeSubMesh(xMesh *sourceMesh,Cube *theCube,bool preserveSource)
{
	////////////////////////////////////////////////////////
	//
	// Our mesh has an mCube member.  This function
	// will process souceMesh and move all geoms/triangles
	// to this mesh that are within the cube's limits.
	//
	// Returns TRUE if any triangles were put in,
	// FALSE if none were.
	//
	////////////////////////////////////////////////////////

	safedelete(mMeshData);
	mMeshData=new MeshData();
	mOwnMeshData=true;

	int aAddedTriangles=0;

	for(GeometryObject *aGeom=(GeometryObject*)sourceMesh->mMeshData->mGeoms.First();aGeom!=NULL;aGeom=(GeometryObject*)sourceMesh->mMeshData->mGeoms.Next())
	{
		GeometryObject *aCurrentGeom=NULL;

		if (aGeom->mOptimized==true)
		{
			//////////////////////////////////////////
			//
			// Importing an optimized geom
			//
			//////////////////////////////////////////
		}
		else
		{
			//////////////////////////////////////////
			//
			// Importing an unoptimized geom
			//
			//////////////////////////////////////////


			EnumPointerlist(Triangle,aT,aGeom->mFaces)
			//for(Triangle *aT=(Triangle*)aGeom->mFaces.First();aT!=NULL;aT=(Triangle*)aGeom->mFaces.Next())
			{
				//gOut.Out("PTR: %d (%d) Fetched: %d (%d)",aGeom->mFaces.lCursorPos[(int)aEnum],aGeom->mFaces.GetCount(),aT
//					,aGeom->mFaces.Get(aGeom->mFaces.lCursorPos[(int)aEnum]));

				if (gMath.IsTriangleInCube(aT,theCube))
				{
					if (aCurrentGeom==NULL)
					{
						aCurrentGeom=new GeometryObject;
						aGeom->mMaterialRef.Dupe(&aCurrentGeom->mMaterialRef);

						if (aGeom->mName!=NULL)
						{
							aCurrentGeom->mName=(char*)malloc(strlen(aGeom->mName)+1);
							strcpy(aCurrentGeom->mName,aGeom->mName);
						}
						else aCurrentGeom->mName=NULL;
						//aCurrentGeom->mMaterialRef=aGeom->mMaterialRef;

						mMeshData->mGeoms.Add(aCurrentGeom);
					}
					Triangle *aNewT=new Triangle;
					memcpy(aNewT,aT,sizeof(Triangle));

					aCurrentGeom->mFaces.Add(aNewT);
					if (preserveSource==false) 
					{
						aGeom->mFaces-=aT;
					}

					aAddedTriangles++;
				}
			}
		}
	}

	if (aAddedTriangles==0) return false;

	mMeshData->mCube.mCorner1.mX=9999999;
	mMeshData->mCube.mCorner1.mY=9999999;
	mMeshData->mCube.mCorner1.mZ=9999999;
	mMeshData->mCube.mCorner2.mX=-9999999;
	mMeshData->mCube.mCorner2.mY=-9999999;
	mMeshData->mCube.mCorner2.mZ=-9999999;

	for(GeometryObject *aGeom=(GeometryObject*)mMeshData->mGeoms.First();aGeom!=NULL;aGeom=(GeometryObject*)mMeshData->mGeoms.Next())
	{
		for(Triangle *aT=(Triangle*)aGeom->mFaces.First();aT!=NULL;aT=(Triangle*)aGeom->mFaces.Next())
		{
			for (int aCount=0;aCount<3;aCount++)
			{
				mMeshData->mCube.mCorner1.mX=min(mMeshData->mCube.mCorner1.mX,aT->mVertex[aCount].mX);
				mMeshData->mCube.mCorner1.mY=min(mMeshData->mCube.mCorner1.mY,aT->mVertex[aCount].mY);
				mMeshData->mCube.mCorner1.mZ=min(mMeshData->mCube.mCorner1.mZ,aT->mVertex[aCount].mZ);

				mMeshData->mCube.mCorner2.mX=max(mMeshData->mCube.mCorner2.mX,aT->mVertex[aCount].mX);
				mMeshData->mCube.mCorner2.mY=max(mMeshData->mCube.mCorner2.mY,aT->mVertex[aCount].mY);
				mMeshData->mCube.mCorner2.mZ=max(mMeshData->mCube.mCorner2.mZ,aT->mVertex[aCount].mZ);
			}
		}
	}

	mMeshData->mMaterials=sourceMesh->mMeshData->mMaterials;
	return true;
}



