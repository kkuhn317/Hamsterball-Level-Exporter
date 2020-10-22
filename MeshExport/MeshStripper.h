// MeshStripper.h: interface for the MeshStripper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHSTRIPPER_H__02F0EF2C_41DC_4436_97F2_E7AAF15DDFA9__INCLUDED_)
#define AFX_MESHSTRIPPER_H__02F0EF2C_41DC_4436_97F2_E7AAF15DDFA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "pointerlist.h"

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

	#define MAX_SHARE	128	// # of edges that can share a vertex

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


#endif // !defined(AFX_MESHSTRIPPER_H__02F0EF2C_41DC_4436_97F2_E7AAF15DDFA9__INCLUDED_)
