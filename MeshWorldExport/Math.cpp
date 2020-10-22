// Math.cpp: implementation of the Math class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Math.h"
#include "console.h"

Math gMath;
extern Console gOut;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Math::Math()
{
	xPI=3.1415925511f;
}

Math::~Math()
{

}

float Math::Sin(float angle)
{
	return (float)sin(angle*xPI/180.0f);
}

float Math::Cos(float angle)
{
	return (float)cos(angle*xPI/180.0f);
}

float Math::GetAngle2D(float theX, float theY, float theCenterX, float theCenterY)
{
	float aAngle;
	float aWorkx,aWorky;

	aWorkx=(float)(theCenterX-theX);
	aWorky=(float)(theCenterY-theY);
	if (aWorky==0) 
	{
		if (aWorkx<=0) return 270.0f;
		return 90.0f;
	}

	aAngle=(float)atan(aWorkx/aWorky);
	aAngle=(float)fabs((aAngle/xPI)*180);

	if (aWorkx>=0 && aWorky<0) return aAngle;
	if (aWorkx<0 && aWorky<0) return (360.0f-aAngle);
	if (aWorkx>=0 && aWorky>0) return (180.0f-aAngle);
	return (180.0f+aAngle);
}

int Math::GetBestCircleDirection(int sourceangle, int destangle) 
{
	int a1,a2;

	a1=sourceangle;
	a2=sourceangle;

	for (int count=0;count<360;count++) 
	{
		if (a1==destangle) return 1;
		if (a2==destangle) return -1;

		if (++a1>=360) a1-=360;
		if (--a2<0) a2+=360;
	}

	return 0;
}



bool Math::DoesLineIntersectPlane(D3DXVECTOR3 &a, D3DXVECTOR3 &b, D3DXPLANE &plane)
{
	D3DXVECTOR3 aIntersection;
    if(NULL!=D3DXPlaneIntersectLine(&aIntersection,&plane,&a,&b)) 
	{
		D3DXVECTOR3 min, max;
		if (a.x > b.x) 
		{ 
			max.x = a.x; 
			min.x = b.x; 
		}
		else 
		{ 
			min.x = a.x; 
			max.x = b.x; 
		}
        
		if (a.y > b.y) 
		{ 
			max.y = a.y; 
			min.y = b.y; 
		}
		else 
		{ 
			min.y = a.y; 
			max.y = b.y; 
		}

		if(a.z > b.z) 
		{ 
			max.z = a.z; 
			min.z = b.z; 
		}
		else 
		{ 
			min.z = a.z; 
			max.z = b.z; 
		}

		if(aIntersection.x >= min.x && aIntersection.x <= max.x &&
			aIntersection.y >= min.y && aIntersection.y <= max.y &&
            aIntersection.z >= min.z && aIntersection.z <= max.z)
            return true;
	}
    return false;
}

bool Math::DoesTriangleIntersectTriangle(D3DXVECTOR3 &u1, D3DXVECTOR3 &u2, D3DXVECTOR3 &u3, D3DXVECTOR3 &v1, D3DXVECTOR3 &v2, D3DXVECTOR3 &v3)
{
	D3DXVECTOR3 aIntersect;
    D3DXPLANE aPlaneU, aPlaneV;

    D3DXPlaneFromPoints(&aPlaneU, &u1, &u2, &u3);
    D3DXPlaneFromPoints(&aPlaneV, &v1, &v2, &v3);

    if(DoesLineIntersectPlane(u1, u2, aPlaneV)) return true;
    if(DoesLineIntersectPlane(u2, u3, aPlaneV)) return true;
    if(DoesLineIntersectPlane(u3, u1, aPlaneV)) return true;

    if(DoesLineIntersectPlane(v1, v2, aPlaneU)) return true;
    if(DoesLineIntersectPlane(v2, v3, aPlaneU)) return true;
    if(DoesLineIntersectPlane(v3, v1, aPlaneU)) return true;
        
    return false;
}

bool Math::DoesTriangleIntersectRectangle(D3DXVECTOR3 &t1, D3DXVECTOR3 &t2, D3DXVECTOR3 &t3, D3DXVECTOR3 &r1, D3DXVECTOR3 &r2, D3DXVECTOR3 &r3, D3DXVECTOR3 &r4) 
{
	if(DoesTriangleIntersectTriangle(t1, t2, t3, r1, r2, r3)) return true;
	if(DoesTriangleIntersectTriangle(t1, t2, t3, r1, r3, r4)) return true;

	return false;
}

bool Math::IsTriangleInCube(Triangle *theTriangle, Cube *theCube)
{
	D3DXVECTOR3 aBoxMin(theCube->mCorner1.mX-1,theCube->mCorner1.mY-1,theCube->mCorner1.mZ-1);
	D3DXVECTOR3 aBoxMax(theCube->mCorner2.mX+1,theCube->mCorner2.mY+1,theCube->mCorner2.mZ+1);

	D3DXVECTOR3 aT1(theTriangle->mVertex[0].mX,theTriangle->mVertex[0].mY,theTriangle->mVertex[0].mZ);
	D3DXVECTOR3 aT2(theTriangle->mVertex[1].mX,theTriangle->mVertex[1].mY,theTriangle->mVertex[1].mZ);
	D3DXVECTOR3 aT3(theTriangle->mVertex[2].mX,theTriangle->mVertex[2].mY,theTriangle->mVertex[2].mZ);

	D3DXVECTOR3 aTriMin=aT1;
	D3DXVECTOR3 aTriMax=aT1;
	D3DXVECTOR3 aTemp;

	for(int a=0;a<2;a++) 
	{
		if(a==0) aTemp = aT2;
		else aTemp = aT3;

		if(aTemp.x <= aTriMin.x) aTriMin.x = aTemp.x;
		if(aTemp.y <= aTriMin.y) aTriMin.y = aTemp.y;
		if(aTemp.z <= aTriMin.z) aTriMin.z = aTemp.z;
		if(aTemp.x >= aTriMax.x) aTriMax.x = aTemp.x;
		if(aTemp.y >= aTriMax.y) aTriMax.y = aTemp.y;
		if(aTemp.z >= aTriMax.z) aTriMax.z = aTemp.z;
    }

	if(aTriMin.x >= aBoxMin.x && aTriMax.x <= aBoxMax.x &&
	   aTriMin.y >= aBoxMin.y && aTriMax.y <= aBoxMax.y &&
	   aTriMin.z >= aBoxMin.z && aTriMax.z <= aBoxMax.z)
		return true;

	if((aTriMin.x > aBoxMax.x && aTriMax.x > aBoxMax.x) ||
       (aTriMin.x < aBoxMin.x && aTriMax.x < aBoxMin.x) ||
       (aTriMin.y > aBoxMax.y && aTriMax.y > aBoxMax.y) ||
       (aTriMin.y < aBoxMin.y && aTriMax.y < aBoxMin.y) ||
       (aTriMin.z > aBoxMax.z && aTriMax.z > aBoxMax.z) ||
       (aTriMin.z < aBoxMin.z && aTriMax.z < aBoxMin.z) )
		return false;

	D3DXVECTOR3 aA(aBoxMin.x, aBoxMin.y, aBoxMin.z),
				aB(aBoxMax.x, aBoxMin.y, aBoxMin.z),
				aC(aBoxMin.x, aBoxMax.y, aBoxMin.z),
				aD(aBoxMax.x, aBoxMax.y, aBoxMin.z),
				aE(aBoxMin.x, aBoxMin.y, aBoxMax.z),
				aF(aBoxMax.x, aBoxMin.y, aBoxMax.z),
				aG(aBoxMin.x, aBoxMax.y, aBoxMax.z),
				aH(aBoxMax.x, aBoxMax.y, aBoxMax.z);

	if(DoesTriangleIntersectRectangle(aT1, aT2, aT3, aA, aC, aD, aB)) return true;
	if(DoesTriangleIntersectRectangle(aT1, aT2, aT3, aH, aF, aE, aG)) return true;
	if(DoesTriangleIntersectRectangle(aT1, aT2, aT3, aE, aG, aC, aA)) return true;
	if(DoesTriangleIntersectRectangle(aT1, aT2, aT3, aD, aH, aF, aB)) return true;
	if(DoesTriangleIntersectRectangle(aT1, aT2, aT3, aC, aG, aH, aD)) return true;
	if(DoesTriangleIntersectRectangle(aT1, aT2, aT3, aF, aB, aA, aE)) return true;

    return false;
}

bool Math::IsPointInTriangle(D3DXVECTOR2 thePoint, Triangle *theTriangle)
{
	float fAB;
	float fBC;
	float fCA;

	fAB=(thePoint.y-theTriangle->mVertex[0].mY)* 
		(theTriangle->mVertex[1].mX-theTriangle->mVertex[0].mX)-
		(thePoint.x-theTriangle->mVertex[0].mX)*
		(theTriangle->mVertex[1].mY-theTriangle->mVertex[0].mY);

	fBC=(thePoint.y-theTriangle->mVertex[1].mY)*
		(theTriangle->mVertex[2].mX-theTriangle->mVertex[1].mX)-
		(thePoint.x-theTriangle->mVertex[1].mX)*
		(theTriangle->mVertex[2].mY-theTriangle->mVertex[1].mY);

	fCA=(thePoint.y-theTriangle->mVertex[2].mY)*
		(theTriangle->mVertex[0].mX-theTriangle->mVertex[2].mX)-
		(thePoint.x-theTriangle->mVertex[2].mX)*
		(theTriangle->mVertex[0].mY-theTriangle->mVertex[2].mY);

	if (fAB*fBC>0 && fBC*fCA>0) return true;
	return false;
}

bool Math::IsPointInTriangle(D3DXVECTOR3 *thePoint, Vertex* theCorner0, Vertex* theCorner1, Vertex* theCorner2)
{
	float fAB;
	float fBC;
	float fCA;

	fAB=(thePoint->z-theCorner0->mZ)* 
		(theCorner1->mX-theCorner0->mX)-
		(thePoint->x-theCorner0->mX)*
		(theCorner1->mZ-theCorner0->mZ);

	fBC=(thePoint->z-theCorner1->mZ)*
		(theCorner2->mX-theCorner1->mX)-
		(thePoint->x-theCorner1->mX)*
		(theCorner2->mZ-theCorner1->mZ);

	fCA=(thePoint->z-theCorner2->mZ)*
		(theCorner0->mX-theCorner2->mX)-
		(thePoint->x-theCorner2->mX)*
		(theCorner0->mZ-theCorner2->mZ);

	if (fAB*fBC>0 && fBC*fCA>0) return true;
	return false;
}

MaterialObject::MaterialObject()
{
	mTexture=NULL;
	memset(&mMaterial,0,sizeof(mMaterial));
	mHasAlpha=false;
	mHasReflection=false;
}

MaterialObject::MaterialObject(float r, float g, float b, float a)
{
	mTexture=NULL;
	memset(&mMaterial,0,sizeof(mMaterial));
	mHasAlpha=false;
	mHasReflection=false;

	SetDiffuse(r,g,b,a);
}


MaterialObject::~MaterialObject()
{
	if (mTexture!=NULL) 
	{
		free(mTexture);
		mTexture=NULL;
	}
}

GeometryObject::GeometryObject()
{
	mOptimized=false;
	mNotifyOnCollision=false;
	mName=NULL;
	mPivot=D3DXVECTOR3(0,0,0);

	mFaces.Clear();
}

GeometryObject::~GeometryObject()
{
	for(Triangle *aT=(Triangle*)mFaces.First();aT!=NULL;aT=(Triangle*)mFaces.Next()) delete aT;
	mFaces.Clear();

	for (TriangleStrip *aTS=(TriangleStrip*)mStrips.First();aTS!=NULL;aTS=(TriangleStrip*)mStrips.Next()) delete aTS;
	mStrips.Clear();

	safefree(mName);

}

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

void MaterialObject::Dupe(MaterialObject *aNew)
{
	memcpy(&aNew->mMaterial,&mMaterial,sizeof(mMaterial));
	if (mTexture==NULL) aNew->mTexture=NULL;
	else
	{
		aNew->mTexture=(char*)malloc(strlen(mTexture)+1);
		strcpy(aNew->mTexture,mTexture);
	}
	aNew->mHasAlpha=mHasAlpha;
	aNew->mHasReflection=mHasReflection;
}



