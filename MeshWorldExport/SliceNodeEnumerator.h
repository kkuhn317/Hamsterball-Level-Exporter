#pragma once
#include <max.h>

class SliceNodeEnumerator :
	public ITreeEnumProc
{
public:
	SliceNodeEnumerator(void);
	virtual ~SliceNodeEnumerator(void);

	virtual int			callback( INode *node );

public:
	Point3				mSlicePoint;
};
