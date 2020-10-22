// MeshStripper.h: interface for the MeshStripper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHSTRIPPER_H__02F0EF2C_41DC_4436_97F2_E7AAF15DDFA9__INCLUDED_)
#define AFX_MESHSTRIPPER_H__02F0EF2C_41DC_4436_97F2_E7AAF15DDFA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
/*

typedef struct Vertex 
{
    float			mX;
	float			mY;
	float			mZ;

    float			mNormalX;
	float			mNormalY;
	float			mNormalZ;

    float			mTextureU;
	float			mTextureV;
} Vertex;

class Triangle
{
public:
	Vertex						mVertex[3];
};


class TriangleStrip
{
public:
	TriangleStrip();
	virtual ~TriangleStrip();

public:
	int							mTriangles;
	int							mVertexListReference;
	int							mSecondaryVertexListReference;
	Vertex						*mVertices;

};

*/
class MeshStripper  
{
public:
	MeshStripper(int maxTriangles);
	virtual ~MeshStripper();

	int					Go(bool MakeStrips=true);
	void				AddVertex(Vertex *v);
	void				AddTriangle(Triangle *t);

	void				GetResults(PointerList *theDestination);
	void				Clear();

protected:

	void				SetFlag(int flag, int value);
	int					NotSharedVertex(int t1, int t2);
	int					FirstSharedVertex(int t1, int t2);
	int					SecondSharedVertex(int t1, int t2);
	int					TriangleUsesVertex(int t, int v);
	int					FollowStrip(int strip[], int *count, int last, int* final);
	int					GetNextStrip(int start, int *cnt, int strip[], bool MakeStrips);
	void				AddEdge(int tnum, int vnum1, int vnum2);
	void				PrepareTriangle(int t);
	int					FindVertex(Vertex *v);
	void				ListVertices(Vertex *v);


public:

	#define MAX_SHARE	256	// # of edges that can share a vertex

	struct StripVertex
	{
		Vertex		mVertex;

		int			mConnCnt;
		int			mConn[MAX_SHARE][2];
	};

	struct StripTriangle
	{
	    int			mVertex[3];	
	    int			mTCount;	
	    int			mTriangle[3];
	    int			mDone;			
	    int			mNext;			
	};

	struct StripEdge
	{
		int			mTriangle[2];
	};

	enum
	{
		STRIP_ERROR						=-1,
		STRIP_DRAWSTRIPOUTLINES			=0,
		STRIP_DRAWCONNECTIVITY,
		STRIP_PRINTSTRIPCONTENTS,
		STRIP_PRINTADDTRIANGLES
	};

	int 					mDrawStripOutlines;
	int 					mDrawConnectivity;
	int 					mPrintStripContents;
	int 					mPrintAddTriangles;

	int						mMaxTriangles;
	int						mMaxStrips;

	int						mVertexCount;
	struct StripVertex		*mVerts;

	int						mTriangleCount;
	struct StripTriangle	*mTriangles;

	int						mEdgeCount;
	struct StripEdge		*mEdges;

	PointerList				mStrips;


protected:

	void					BeginStrip(StripVertex *v1, StripVertex *v2);
	void					AddStrip(StripVertex *v);
	void					EndStrip();

	TriangleStrip			*mCurrentStrip;
	PointerList				mVertexList;

//		drawBeginStrip(&verts[sv[0]], &verts[sv[1]]);
//      for(i = 2; i < sl; i++) drawContStrip(&verts[sv[i]]);
//		drawEndStrip();

};

/*
class SpaceCoordinate
{
public:
	float						mX;
	float						mY;
	float						mZ;
};


class Cube
{
public:
	Cube();
	~Cube();

	void						Fit(int theX, int theY, int theZ);
	void						Reset();

public:
	SpaceCoordinate				mCorner1;
	SpaceCoordinate				mCorner2;
};

class MaterialObject
{
public:
	MaterialObject();
	MaterialObject(float r, float g, float b, float a=1.0);
	virtual ~MaterialObject();

public:
	D3DMATERIAL8				mMaterial;
	char						*mTexture;
	bool						mHasAlpha;
	bool						mHasReflection;

	inline void					SetDiffuse(float r, float g, float b, float a=1.0) {mMaterial.Diffuse.r=r;mMaterial.Diffuse.g=g;mMaterial.Diffuse.b=b;mMaterial.Diffuse.a=a;SetAlphaBit();}
	inline void					SetAmbient(float r, float g, float b, float a=1.0) {mMaterial.Ambient.r=r;mMaterial.Ambient.g=g;mMaterial.Ambient.b=b;mMaterial.Ambient.a=a;SetAlphaBit();}
	inline void					SetEmissive(float r, float g, float b, float a=1.0) {mMaterial.Emissive.r=r;mMaterial.Emissive.g=g;mMaterial.Emissive.b=b;mMaterial.Emissive.a=a;SetAlphaBit();}
	inline void					SetSpecular(float r, float g, float b, float a=1.0) {mMaterial.Specular.r=r;mMaterial.Specular.g=g;mMaterial.Specular.b=b;mMaterial.Specular.a=a;SetAlphaBit();}
	inline void					SetAlphaBit() {mHasAlpha=(mMaterial.Diffuse.a!=1.0);}
};

class GeometryObject
{
public:
	GeometryObject();
	virtual ~GeometryObject();

public:
	int							mMaterialRef;

	PointerList					mFaces;
	PointerList					mStrips;

	bool						mOptimized;
	bool						mNotifyOnCollision;

	char						*mName;
	SpaceCoordinate				mPivot;
};
*/

#endif // !defined(AFX_MESHSTRIPPER_H__02F0EF2C_41DC_4436_97F2_E7AAF15DDFA9__INCLUDED_)
