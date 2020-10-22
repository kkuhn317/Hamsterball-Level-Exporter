// WorldMesh.h: interface for the WorldMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDMESH_H__6268C77E_572E_4FE1_9739_6CE3E11C7B17__INCLUDED_)
#define AFX_WORLDMESH_H__6268C77E_572E_4FE1_9739_6CE3E11C7B17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "Mesh.h"

class MeshWorldExport;

class WorldOptimizeKit
{
public:
	WorldOptimizeKit();
	virtual ~WorldOptimizeKit();

	int					AddVertexList(Vertex *theList, int theCount);


public:
	Vertex				*mVertexList;
	int					mVertexCount;
};

class WorldMesh : public xMesh  
{
public:
	WorldMesh();
	WorldMesh(char *theFilename);
	virtual ~WorldMesh();

	virtual void				Generate(int xCount, int yCount, int zCount, bool shouldOptimize=true);				
	virtual void				SliceGenerate(int theStep, bool shouldOptimize=true);

	virtual bool				MakeSubMesh(WorldMesh *sourceMesh, Cube *theCube, bool preserveSource=false);
	void						WorldOptimize();
	void						WorldOptimizeDump(WorldOptimizeKit *theKit);

	void						GetSubMeshes(PointerList *aResultList);

	void						Dump(MeshWorldExport *mExporter);


public:

	PointerList					mSubMeshes;
	bool						mSubdivided;

	float						mSizeX;
	float						mSizeY;
	float						mSizeZ;

	WorldOptimizeKit			mKit;


};

#endif // !defined(AFX_WORLDMESH_H__6268C77E_572E_4FE1_9739_6CE3E11C7B17__INCLUDED_)
