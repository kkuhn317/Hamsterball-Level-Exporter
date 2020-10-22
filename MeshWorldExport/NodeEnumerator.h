// NodeEnumerator.h: interface for the NodeEnumerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODEENUMERATOR_H__CBFF38C9_6743_4895_A1CC_D6C9F32C5AA2__INCLUDED_)
#define AFX_NODEENUMERATOR_H__CBFF38C9_6743_4895_A1CC_D6C9F32C5AA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <max.h>
//#include <interfaceserver.h>
//#include <baseinterface.h>
//#include <inode.h>
//#include <maxapi.h>

class MeshWorldExport;

class NodeEnumerator : public ITreeEnumProc  
{
public:
	NodeEnumerator();
	virtual ~NodeEnumerator();

	Point3		GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);

public:
	virtual int callback( INode *node );

	MeshWorldExport					*mExporter;


};

#endif // !defined(AFX_NODEENUMERATOR_H__CBFF38C9_6743_4895_A1CC_D6C9F32C5AA2__INCLUDED_)
