// Math.h: interface for the Math class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATH_H__FDB9596F_2E11_4E20_A35E_8CDCC9705490__INCLUDED_)
#define AFX_MATH_H__FDB9596F_2E11_4E20_A35E_8CDCC9705490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PointerList.h"

class Texture;
class Triangle;
class Cube;

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

#define RAD(x) (float)((float)x*3.141592654f/180.0f)

class Math  
{
public:
	Math();
	virtual ~Math();

	const static int mPlaneBacksize=0x000001;
	const static int mPlaneFront=0x000010;
	const static int mOnPlane=0x000100;

	float				Sin(float angle);
	float				Cos(float angle);
	float				GetAngle2D(float theX, float theY, float theCenterX, float theCenterY);
	int					GetBestCircleDirection(int sourceangle, int destangle);
	bool				DoesLineIntersectPlane(D3DXVECTOR3 &a, D3DXVECTOR3 &b, D3DXPLANE &plane);
	bool				DoesTriangleIntersectTriangle(D3DXVECTOR3 &u1, D3DXVECTOR3 &u2, D3DXVECTOR3 &u3, D3DXVECTOR3 &v1, D3DXVECTOR3 &v2, D3DXVECTOR3 &v3);
	bool				DoesTriangleIntersectRectangle(D3DXVECTOR3 &t1, D3DXVECTOR3 &t2, D3DXVECTOR3 &t3, D3DXVECTOR3 &r1, D3DXVECTOR3 &r2, D3DXVECTOR3 &r3, D3DXVECTOR3 &r4);
	bool				IsTriangleInCube(Triangle *theTriangle, Cube *theCube);
	bool				IsPointInTriangle(D3DXVECTOR2 thePoint, Triangle *theTriangle);
	bool				IsPointInTriangle(D3DXVECTOR3 *thePoint, Vertex* theCorner1, Vertex* theCorner2, Vertex* theCorner3);

	inline float		Distance(D3DXVECTOR3 aFirst, D3DXVECTOR3 aSecond) {return D3DXVec3Length(&D3DXVECTOR3(aSecond-aFirst));}

public:						
	float				xPI;
};

typedef struct Vertex2D
{
    float			mX;
	float			mY;
	float			mZ;
	float			mRHW;

	long			mDiffuse;

    float			mTextureU;
	float			mTextureV;
} Vertex2D;

typedef struct VertexManual
{
    float			mX;
	float			mY;
	float			mZ;

    float			mNormalX;
	float			mNormalY;
	float			mNormalZ;

	DWORD			mDiffuse;

    float			mTextureU;
	float			mTextureV;
} VertexManual;

typedef struct XRect
{
	float			mLeft;
	float			mTop;
	float			mRight;
	float			mBottom;
} XRect;


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

	void						Dupe(MaterialObject *aNew);
};

class GeometryObject
{
public:
	GeometryObject();
	virtual ~GeometryObject();

public:
	MaterialObject				mMaterialRef;

	PointerList					mFaces;
	PointerList					mStrips;

	bool						mOptimized;
	bool						mNotifyOnCollision;

	char						*mName;
	D3DXVECTOR3					mPivot;
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

class SpaceCoordinate
{
public:
	float						mX;
	float						mY;
	float						mZ;
};

class TextureCoordinate
{
public:
	float						mU;
	float						mV;
};

class Triangle
{
public:
	Vertex						mVertex[3];
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





#endif // !defined(AFX_MATH_H__FDB9596F_2E11_4E20_A35E_8CDCC9705490__INCLUDED_)
