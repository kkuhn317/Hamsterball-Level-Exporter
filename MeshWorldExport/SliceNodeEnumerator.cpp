#include "slicenodeenumerator.h"

#include "NodeEnumerator.h"
#include "meshworldexport.h"
#include <meshadj.h>
#include <stdmat.h>
#include "meshstripper.h"
#include "worldmesh.h"
#include <modstack.h>

//////////////////////////////////////////////
// Required .H headerfiles 
// stdio.h
// stdlib.h
// psdk_inc/_ws1_undef.h
/////////////////////////////////////////////

SliceNodeEnumerator::SliceNodeEnumerator(void)
{
}

SliceNodeEnumerator::~SliceNodeEnumerator(void)
{
}

int SliceNodeEnumerator::callback( INode *node )
{
	if (!node->IsGroupHead())
	{
		if (node->Renderable())
		{
			Object *aWorkObject=node->GetObjectRef();//node->EvalWorldState(0).obj;

	        IDerivedObject* aDObject=CreateDerivedObject(aWorkObject);

			Matrix3 aMX1,aMX2;
			aMX1.IdentityMatrix();
			aMX2.IdentityMatrix();
			Quat aRotate;

			aMX1=node->GetObjectTM(0);

			Modifier *aSlice;
				
			aSlice=(Modifier*)CreateInstance(OSM_CLASS_ID,Class_ID(0x2e3741fe, 0x12702ab7));
			aSlice->Move(0,aMX1,aMX2,mSlicePoint);
		    aDObject->AddModifier(aSlice);

			aSlice=(Modifier*)CreateInstance(OSM_CLASS_ID,Class_ID(0x2e3741fe, 0x12702ab7));
			aRotate.x=0;
			aRotate.y=1;
			aRotate.z=0;
			aRotate.w=1;
			aSlice->Rotate(0,aMX1,aMX2,aRotate);
			aSlice->Move(0,aMX1,aMX2,mSlicePoint);
		    aDObject->AddModifier(aSlice);

			aSlice=(Modifier*)CreateInstance(OSM_CLASS_ID,Class_ID(0x2e3741fe, 0x12702ab7));
			aRotate.x=0;
			aRotate.y=1;
			aRotate.z=0;
			aRotate.w=1;
			aSlice->Rotate(0,aMX1,aMX2,aRotate);

			aRotate.x=0;
			aRotate.y=0;
			aRotate.z=1;
			aRotate.w=1;
			aSlice->Rotate(0,aMX1,aMX2,aRotate);
			aSlice->Move(0,aMX1,aMX2,mSlicePoint);
		    aDObject->AddModifier(aSlice);

	        node->SetObjectRef(aDObject);
		}
	}

	return TREE_CONTINUE;
}

