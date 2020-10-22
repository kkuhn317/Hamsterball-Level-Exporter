// Mesh.h: interface for the Mesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESH_H__9F69D19C_3203_447E_BFD0_D0CA995B4B7B__INCLUDED_)
#define AFX_MESH_H__9F69D19C_3203_447E_BFD0_D0CA995B4B7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Pointerlist.h"
#include "math.h"

class MaterialObject;


class MeshData
{
public:
	MeshData();
	virtual ~MeshData();

public:
	void						Optimize();
	void						OptimizeRoutine();
	void						SetupVertexBuffer();

public:
	
	int							mMaterialCount;
	MaterialObject				*mMaterials;
	PointerList					mGeoms;
	int							mVertexCount;
	Vertex						*mVertexList;

	bool						mOwnResources;
	bool						mOptimized;	

	Cube						mCube;
};

class xMesh  
{
public:
	xMesh();
	xMesh(MeshData *theMeshData);
	xMesh(char *theFilename);
	virtual ~xMesh();

	virtual bool				MakeSubMesh(xMesh *sourceMesh, Cube *theCube, bool preserveSource=false);

public:

	
	//
	// This is mesh data, which *should* be stored potentially 'elsewhere'
	//


	MeshData					*mMeshData;

	bool						mImported;
	bool						mOwnMeshData;


};

#endif // !defined(AFX_MESH_H__9F69D19C_3203_447E_BFD0_D0CA995B4B7B__INCLUDED_)
