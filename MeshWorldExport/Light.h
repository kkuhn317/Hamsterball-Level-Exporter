#pragma once

#include "meshworldexport.h"

class xLight
{
public:
	xLight(void);
	virtual ~xLight(void);

	enum
	{
		DISTANTLIGHT = 0,
		BULBLIGHT,
		SPOTLIGHT,
		MAXLIGHT
	};

public:

	int				mType;
	Point3			mPosition;
	Point3			mLookat;
	Point3			mColor;
};

class xPoint
{
public:

	char			mName[256];
	Point3			mPosition;
	Point3			mRotation;

	char			mHasMaterial;
	MaterialObject	mMaterial;
	char			mTextureName[256];
};

class xSpline
{
public:
	char			mName[256];
	PointerList		mPointList;
};
