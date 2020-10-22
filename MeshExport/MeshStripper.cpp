// MeshStripper.cpp: implementation of the MeshStripper class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "pointerlist.h"



#include "MeshStripper.h"

TriangleStrip::TriangleStrip()
{
	mVertexListReference=NULL;
	mVertices=NULL;
}

TriangleStrip::~TriangleStrip()
{
	if (mVertices!=NULL)
	{
		delete [] mVertices;
		mVertices=NULL;
	}
}

Cube::Cube()
{
	Reset();
}

Cube::~Cube()
{
}

void Cube::Reset()
{
	mCorner1.mX=9999999;
	mCorner1.mY=9999999;
	mCorner1.mZ=9999999;

	mCorner2.mX=-9999999;
	mCorner2.mY=-9999999;
	mCorner2.mZ=-9999999;
}

void Cube::Fit(int theX, int theY, int theZ)
{
	if (theX<mCorner1.mX) mCorner1.mX=theX;
	if (theY<mCorner1.mY) mCorner1.mY=theY;
	if (theZ<mCorner1.mZ) mCorner1.mZ=theZ;

	if (theX>mCorner2.mX) mCorner2.mX=theX;
	if (theY>mCorner2.mY) mCorner2.mY=theY;
	if (theZ>mCorner2.mZ) mCorner2.mZ=theZ;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MeshStripper::MeshStripper(int maxTriangles)
{
	mDrawStripOutlines=0;
	mDrawConnectivity=0;
	mPrintStripContents=0;
	mPrintAddTriangles=0;

	mMaxTriangles=maxTriangles;
	mMaxStrips=mMaxTriangles+2;

	mVerts=new struct StripVertex[mMaxTriangles*3];
	mTriangles=new struct StripTriangle[mMaxTriangles];
	mEdges=new struct StripEdge[mMaxTriangles*3];

	mVertexCount=0;
	mTriangleCount=0;
	mEdgeCount=0;

	mStrips.Clear();
}

MeshStripper::~MeshStripper()
{
	if (mVerts!=NULL) 
	{
		delete [] mVerts;
		mVerts=NULL;
	}
	if (mTriangles!=NULL)
	{
		delete [] mTriangles;
		mTriangles=NULL;
	}
	if (mEdges!=NULL)
	{
		delete [] mEdges;
		mEdges=NULL;
	}

	mStrips.Clear();
}


void MeshStripper::SetFlag(int flag, int value)
{
    switch(flag)
    {
        case STRIP_DRAWSTRIPOUTLINES:
	    mDrawStripOutlines = value; break;

        case STRIP_DRAWCONNECTIVITY:
	    mDrawConnectivity = value; break;

        case STRIP_PRINTSTRIPCONTENTS:
	    mPrintStripContents = value; break;

        case STRIP_PRINTADDTRIANGLES:
	    mPrintAddTriangles = value; break;
    }
}

int MeshStripper::NotSharedVertex(int t1, int t2)
{
	if ((mTriangles[t1].mVertex[0]!=mTriangles[t2].mVertex[0]) &&
		(mTriangles[t1].mVertex[0]!=mTriangles[t2].mVertex[1]) &&
		(mTriangles[t1].mVertex[0]!=mTriangles[t2].mVertex[2]))
	{
		return mTriangles[t1].mVertex[0];
	}

	if ((mTriangles[t1].mVertex[1]!=mTriangles[t2].mVertex[0]) &&
		(mTriangles[t1].mVertex[1]!=mTriangles[t2].mVertex[1]) &&
		(mTriangles[t1].mVertex[1]!=mTriangles[t2].mVertex[2]))
	{
		return mTriangles[t1].mVertex[1];
	}

	return mTriangles[t1].mVertex[2];
}

int MeshStripper::FirstSharedVertex(int t1, int t2)
{
	if ((mTriangles[t1].mVertex[0]==mTriangles[t2].mVertex[0]) ||
		(mTriangles[t1].mVertex[0]==mTriangles[t2].mVertex[1]) ||
		(mTriangles[t1].mVertex[0]==mTriangles[t2].mVertex[2]))
	{
		return mTriangles[t1].mVertex[0];
	}

	if ((mTriangles[t1].mVertex[1]==mTriangles[t2].mVertex[0]) ||
		(mTriangles[t1].mVertex[1]==mTriangles[t2].mVertex[1]) ||
		(mTriangles[t1].mVertex[1]==mTriangles[t2].mVertex[2]))
	{
		return mTriangles[t1].mVertex[1];
	}

	return STRIP_ERROR;
}

int MeshStripper::SecondSharedVertex(int t1, int t2)
{
    if ((mTriangles[t1].mVertex[2] == mTriangles[t2].mVertex[0])  ||
        (mTriangles[t1].mVertex[2] == mTriangles[t2].mVertex[1]) ||
        (mTriangles[t1].mVertex[2] == mTriangles[t2].mVertex[2]))
    {
        return(mTriangles[t1].mVertex[2]);
    }

    if ((mTriangles[t1].mVertex[1] == mTriangles[t2].mVertex[0]) ||
        (mTriangles[t1].mVertex[1] == mTriangles[t2].mVertex[1]) ||
        (mTriangles[t1].mVertex[1] == mTriangles[t2].mVertex[2]))
    {
        return(mTriangles[t1].mVertex[1]);
    }
	return STRIP_ERROR;
}

int MeshStripper::TriangleUsesVertex(int t, int v)
{
    if((v == mTriangles[t].mVertex[0]) || (v == mTriangles[t].mVertex[1]) || (v == mTriangles[t].mVertex[2])) return(1);
    return(0);
}

int MeshStripper::FollowStrip(int strip[], int *count, int last, int* final)
{
    int	i;
    int	next;
	int winding=0;


    while (*count<mMaxStrips)
	{
		next = -1;
		if (*count==0)
		{
		    for(i=0;i<mTriangles[last].mTCount;i++)
			{
				if(!mTriangles[mTriangles[last].mTriangle[i]].mDone)
				{
					next = mTriangles[last].mTriangle[i];
					break;
				}
			}
			if (next!=-1) 
			{
				strip[0]=NotSharedVertex(last, next);
				strip[1]=FirstSharedVertex(last, next);

				if (strip[1]==STRIP_ERROR)
				{
					return strip[1];
				}

				strip[2]=SecondSharedVertex(last, next);

				if (strip[2]==STRIP_ERROR)
				{
					return strip[2];
				}

				if ((strip[0] == mTriangles[last].mVertex[0]) &&
					(strip[1] == mTriangles[last].mVertex[1]) &&
					(strip[2] == mTriangles[last].mVertex[2]))
				{
					winding = 1; // anti-clock
				}
				else if ((strip[0] == mTriangles[last].mVertex[1]) &&
						 (strip[1] == mTriangles[last].mVertex[0]) &&
						 (strip[2] == mTriangles[last].mVertex[2]))
				{
					winding = 0; // clock
				}
				else if ((strip[0] == mTriangles[last].mVertex[2]) &&
						 (strip[1] == mTriangles[last].mVertex[0]) &&
						 (strip[2] == mTriangles[last].mVertex[1]))
				{
					winding = 1; // anti-clock
				}
				else
				{
					MessageBox(0,"Could not determine WINDING","Severe Error",MB_OK);
					return STRIP_ERROR;
				}

			*count=3;
			}
			else
			{
				strip[0] = mTriangles[last].mVertex[0];
				strip[1] = mTriangles[last].mVertex[1];
				strip[2] = mTriangles[last].mVertex[2];

				winding = 1; // anti-clock
				*count = 3;
			}
		}
		else
		{
			for(i=0;i<mTriangles[last].mTCount;i++)
			{
				if(!mTriangles[mTriangles[last].mTriangle[i]].mDone &&
					TriangleUsesVertex(mTriangles[last].mTriangle[i], strip[*count - 2]) &&
					TriangleUsesVertex(mTriangles[last].mTriangle[i], strip[*count - 1]))
				{
					next = mTriangles[last].mTriangle[i];
					break;
				}
			}

		}


		if(next == -1) 
		{
			break;
		}

		strip[*count] = NotSharedVertex(next, last);
		winding ^= 1;
	
		*count = *count + 1;
	
		mTriangles[next].mDone = 1;
	
		last = next;

    }
	*final = last;
	return winding; // no error
}

int MeshStripper::GetNextStrip(int start, int *cnt, int strip[], bool MakeStrips)
{
    int	i;
    int	*istrip=new int[mMaxStrips];
    int final;
    int min, mincnt;
	int winding;


	if (MakeStrips)
	{

	    mincnt=4;
	    min=-1;
	    for(i=0;i<mTriangleCount;i++)
		{
			if((!mTriangles[i].mDone) && (mTriangles[i].mTCount<mincnt))
			{
				min=i;
				mincnt=mTriangles[i].mTCount;
			}
		}

		if(min != -1)
		{
		start = min;
		}
		else
		{
			delete [] istrip;
			return STRIP_ERROR;
		}	

		mTriangles[start].mDone=1;
		*cnt = 0;

		winding = FollowStrip(istrip, cnt, start, &final);

		if (winding==STRIP_ERROR)
		{
			delete [] istrip;
			return winding;
		}
		for(i = 0; i < *cnt; i++)
		{
			strip[i] = istrip[*cnt - i - 1];
		}

		// winding should be inverted by strip reversal
		winding ^= 1;

		// the value of winding is the value of the winding order
		// for the final triangle in the strip, which is the start
		// after the reversal of the pass 1 data

		// if winding is clockwise, back up one and undo "done" marker on last triangle...
		// backing up should flip winding automatically
		if (winding == 0)
		{
			// test for infinite looping...
			if (final != start)
			{
				// recopy istrip into strip, starting at i=1 to allow
				// for skipping old end (new start)
				for(i = 1; i < *cnt; i++)
				{
    		    strip[i-1] = istrip[*cnt - i - 1];
				}

				// back up count
				*cnt -= 1;

				// mark triangle corresponding to backed up vertex as not done
				mTriangles[final].mDone = 0;

				// pass 2 - follow reversed strip out in the other direction
				winding = FollowStrip(strip, cnt, start, &final);

				if (winding == STRIP_ERROR)
				{
					delete [] istrip;
					return winding;
				}
			}
			else
			{
				// rebuild single triangle strip in anti-clockwise order
				strip[0] = mTriangles[start].mVertex[0];
				strip[1] = mTriangles[start].mVertex[1];
				strip[2] = mTriangles[start].mVertex[2];
			}
		}
		else
		{
			// pass 2 - follow reversed strip out in the other direction
			winding = FollowStrip(strip, cnt, start, &final);

			if (winding == STRIP_ERROR)
			{
				delete [] istrip;
				return winding;
			}
		}
	}
	else
	{
	    *cnt = 3;
	    strip[0] = mTriangles[start].mVertex[0];
	    strip[1] = mTriangles[start].mVertex[1];
	    strip[2] = mTriangles[start].mVertex[2];
	    mTriangles[start].mDone = 1;
	}

	delete [] istrip;
	return 0; // no error
}

int MeshStripper::Go(bool MakeStrips)
{
    int		done;
    int		first;
    int		sl;
    int		i;
    int		meshcnt;
    int		ret;

	int 	*sv=new int[mMaxStrips*3];//[MAX_STRIP*3];

    done = 0;
    meshcnt = 0;

    do
	{
/*
gOut.Out("e1");
gOut.Out("TCOUNT: %d",mTriangleCount);
		for(first = 0; mTriangles[first].mDone && first < mTriangleCount; first++)
		{
			gOut.Out("TCOUNT: [%d]",first);
			gOut.Out("TCOUNT: [%d] %d",first,mTriangles[first].mDone);
		}
gOut.Out("e2");
*/
		for (first=0;first<mTriangleCount;first++) if (!mTriangles[first].mDone) break;

		if(first == mTriangleCount)
		{
			//
			// No more triangles
			//
			break;	
		}

		ret = GetNextStrip(first, &sl, sv, MakeStrips);

		if (ret == STRIP_ERROR)
		{
			return ret;
		}

		meshcnt++;

		BeginStrip(&mVerts[sv[0]],&mVerts[sv[1]]);
		for (i=2;i<sl;i++) AddStrip(&mVerts[sv[i]]);
		EndStrip();

    } while(!done);


    return(meshcnt);
}

void MeshStripper::AddEdge(int tnum, int vnum1, int vnum2)
{
    struct StripTriangle *t, *t2;
    struct StripVertex *v1, *v2;
    int	e;

    t = &mTriangles[tnum];
    v1 = &mVerts[vnum1];
    v2 = &mVerts[vnum2];

    for(e = 0; (e < v1->mConnCnt) && (v1->mConn[e][0] != vnum2); e++);

    if(e < v1->mConnCnt)
	{
		e = v1->mConn[e][1];
		mEdges[e].mTriangle[1] = tnum;
		t2 = &mTriangles[mEdges[e].mTriangle[0]];
		t->mTriangle[t->mTCount++] = mEdges[e].mTriangle[0];
		t2->mTriangle[t2->mTCount++] = tnum;
    }
	else
	{
	    /* have to make new edge */

        e = mEdgeCount++;
        v1->mConn[v1->mConnCnt][0] = vnum2;
        v1->mConn[v1->mConnCnt++][1] = e;
        v2->mConn[v2->mConnCnt][0] = vnum1;
        v2->mConn[v2->mConnCnt++][1] = e;
        mEdges[e].mTriangle[0] = tnum;
    }
}

void MeshStripper::PrepareTriangle(int t) 
{
	AddEdge(t, mTriangles[t].mVertex[0], mTriangles[t].mVertex[1]);
	AddEdge(t, mTriangles[t].mVertex[1], mTriangles[t].mVertex[2]);
	AddEdge(t, mTriangles[t].mVertex[2], mTriangles[t].mVertex[0]);
}

void MeshStripper::AddVertex(Vertex *v)
{
	if (FindVertex(v)==-1)
	{
		mVerts[mVertexCount].mVertex.mX=v->mX;
		mVerts[mVertexCount].mVertex.mY=v->mY;
		mVerts[mVertexCount].mVertex.mZ=v->mZ;

		mVerts[mVertexCount].mVertex.mNormalX=v->mNormalX;
		mVerts[mVertexCount].mVertex.mNormalY=v->mNormalY;
		mVerts[mVertexCount].mVertex.mNormalZ=v->mNormalZ;
	
		mVerts[mVertexCount].mVertex.mTextureU=v->mTextureU;
		mVerts[mVertexCount].mVertex.mTextureV=v->mTextureV;
		mVerts[mVertexCount].mConnCnt=0;

		mVertexCount++;
	}
}

void MeshStripper::AddTriangle(Triangle *t)
{
	mTriangles[mTriangleCount].mVertex[0]=FindVertex(&t->mVertex[0]);
	mTriangles[mTriangleCount].mVertex[1]=FindVertex(&t->mVertex[1]);
	mTriangles[mTriangleCount].mVertex[2]=FindVertex(&t->mVertex[2]);

	mTriangles[mTriangleCount].mDone=0;
	mTriangles[mTriangleCount].mTCount=0;
	PrepareTriangle(mTriangleCount);

	mTriangleCount++;
}

int	MeshStripper::FindVertex(Vertex *v)
{
	for (int aCount=0;aCount<mVertexCount;aCount++)
	{
		if (mVerts[aCount].mVertex.mX==v->mX &&
			mVerts[aCount].mVertex.mY==v->mY &&
			mVerts[aCount].mVertex.mZ==v->mZ &&

			mVerts[aCount].mVertex.mNormalX==v->mNormalX &&
			mVerts[aCount].mVertex.mNormalY==v->mNormalY &&
			mVerts[aCount].mVertex.mNormalZ==v->mNormalZ &&

			mVerts[aCount].mVertex.mTextureU==v->mTextureU &&
			mVerts[aCount].mVertex.mTextureV==v->mTextureV) 
		{
			return aCount;
		}
	}

	return -1;
}

void MeshStripper::ListVertices(Vertex *v)
{
	for (int aCount=0;aCount<mVertexCount;aCount++)
	{
		if (mVerts[aCount].mVertex.mX==v->mX &&
			mVerts[aCount].mVertex.mY==v->mY &&
			mVerts[aCount].mVertex.mZ==v->mZ)
		{

			if (mVerts[aCount].mVertex.mNormalX==v->mNormalX &&
				mVerts[aCount].mVertex.mNormalY==v->mNormalY &&
				mVerts[aCount].mVertex.mNormalZ==v->mNormalZ)
			{

				if (mVerts[aCount].mVertex.mTextureU==v->mTextureU &&
					mVerts[aCount].mVertex.mTextureV==v->mTextureV)
				{
				}
			}
		}
	}
}


void MeshStripper::BeginStrip(StripVertex *v1, StripVertex *v2)
{
	mVertexList.Clear();
	mVertexList.Add(v1);
	mVertexList.Add(v2);

	mCurrentStrip=new TriangleStrip;
	mCurrentStrip->mTriangles=0;
}

void MeshStripper::AddStrip(StripVertex *v)
{
	mVertexList.Add(v);
	mCurrentStrip->mTriangles++;
}

void MeshStripper::EndStrip()
{
	int aVPtr=0;

	mCurrentStrip->mVertices=new Vertex[mVertexList.GetCount()];
	Vertex *aVertexPtr;

	for (struct StripVertex *aSV=(struct StripVertex*)mVertexList.First();aSV!=NULL;aSV=(struct StripVertex*)mVertexList.Next())
	{
		aVertexPtr=&mCurrentStrip->mVertices[aVPtr];

		aVertexPtr->mX=aSV->mVertex.mX;
		aVertexPtr->mY=aSV->mVertex.mY;
		aVertexPtr->mZ=aSV->mVertex.mZ;

		aVertexPtr->mNormalX=aSV->mVertex.mNormalX;
		aVertexPtr->mNormalY=aSV->mVertex.mNormalY;
		aVertexPtr->mNormalZ=aSV->mVertex.mNormalZ;

		aVertexPtr->mTextureU=aSV->mVertex.mTextureU;
		aVertexPtr->mTextureV=aSV->mVertex.mTextureV;

		aVPtr++;
	}

	mStrips.Add(mCurrentStrip);
}

void MeshStripper::GetResults(PointerList *theDestination)
{
	theDestination->Clear();

	for (TriangleStrip *aStrip=(TriangleStrip*)mStrips.First();aStrip!=NULL;aStrip=(TriangleStrip*)mStrips.Next())
	{
		theDestination->Add(aStrip);
	}
}

void MeshStripper::Clear()
{
	for (TriangleStrip *aStrip=(TriangleStrip*)mStrips.First();aStrip!=NULL;aStrip=(TriangleStrip*)mStrips.Next()) delete aStrip;
	mStrips.Clear();
}











