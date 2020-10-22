// NodeEnumerator.cpp: implementation of the NodeEnumerator class.
//
//////////////////////////////////////////////////////////////////////

#include "NodeEnumerator.h"
#include "meshstripper.h"
#include "meshexport.h"
#include <meshadj.h>
#include <stdmat.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NodeEnumerator::NodeEnumerator()
{

}

NodeEnumerator::~NodeEnumerator()
{

}

Point3 NodeEnumerator::GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
	Face* f = &mesh->faces[faceNo];
	DWORD smGroup = f->smGroup;
	int numNormals;
	Point3 vertexNormal;
	
	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL) {
		vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) {
		// If there is only one vertex is found in the rn member.
		if (numNormals == 1) {
			vertexNormal = rv->rn.getNormal();
		}
		else {
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++) {
				if (rv->ern[i].getSmGroup() & smGroup) {
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else {
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = mesh->getFaceNormal(faceNo);
	}
	
	return vertexNormal;
}


int NodeEnumerator::callback( INode *node )
{

	if (!node->IsGroupHead())
	{
		//
		// Okay, this node is not a group, which means it's an object of
		// some sort.  
		//

		/*

		//
		// This little chunk of code will help tell you if you're dealing with bones or
		// meshes or whatnot.  A bone is a GEOMOBJECT, but then you have to check the classID
		//

		ObjectState aObjectState=node->EvalWorldState(0);
		if (aObjectState.obj->SuperClassID()==GEOMOBJECT_CLASS_ID)
		{
			char aMsg[256];
			sprintf(aMsg,"Node: %s = %d",node->GetName(),aObjectState.obj->ClassID());
			MessageBox(0,aMsg,"HEY",MB_OK);
		}
		*/


		if (node->Renderable())
		{
			Object *aObject=node->EvalWorldState((GetCOREInterface())->GetTime()).obj;
//			Object *aObject=node->EvalWorldState(0).obj;
			if (aObject->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID,0)))
			{
				TriObject *aTriObject=(TriObject*)aObject->ConvertToType(0,Class_ID(TRIOBJ_CLASS_ID,0));
				Mesh aMesh=aTriObject->GetMesh();
				aMesh.RemoveDegenerateFaces();
			
				aMesh.buildNormals();

				int aTextureVertices=aMesh.getNumTVerts();

				char *aObjectName=node->GetName();
				Mtl *aObjectMaterial=node->GetMtl();


				// Prepare a triangle stripper.
				// We want to dump all the faces from this object
				// into the stripper for generation of triangle
				// strips.

				Matrix3 aPivotMatrix=node->GetNodeTM(0);
				Matrix3 aWorldMatrix=node->GetObjectTM(0);

				Point3 aPivotPoint=aPivotMatrix.GetTrans();
				Point3 aWorldPoint=aWorldMatrix.GetTrans();

				PointerList aTriangleList;

				for (int aFaceCount=0;aFaceCount<aMesh.numFaces;aFaceCount++)
				{
					Face aFace=aMesh.faces[aFaceCount];
					TVFace aTexFace;

					if (aTextureVertices>0) aTexFace=aMesh.tvFace[aFaceCount];


					Triangle *aTriangle=new Triangle;

					for (int aVCount=0;aVCount<3;aVCount++)
					{

						//
						// Get the vertex's position in 'real' space...
						// Note that 3DSMax does CULLCW... we want CULLCCW, so
						// we reverse the vertices we fetch.
						//

						Point3 aVertex=aMesh.verts[aFace.v[2-aVCount]];
						Point3 aRealVertex=aWorldMatrix.PointTransform(aVertex);
						//
						// Get the vertex's normal...
						//

						Point3 aVertexNormal=GetVertexNormal(&aMesh,aFaceCount,aMesh.getRVertPtr(aFace.v[2-aVCount]));
						Point3 aRealVertexNormal=aWorldMatrix.VectorTransform(aVertexNormal);

						//
						// Get the vertex's TU coordinates...
						// If we haven't mapped this object, we'll
						// just use 0,0,0
						//

						UVVert aTextureVertex;
						if (aTextureVertices>0) 
						{
							aTextureVertex=aMesh.tVerts[aTexFace.t[2-aVCount]];
						}
						else
						{
							aTextureVertex.x=0;
							aTextureVertex.y=0;
							aTextureVertex.z=0;

						}

						// Okay, consolidate all of that, and dump this triangle into the
						// mesh stripper to prepare for a stripgen.

						aTriangle->mVertex[aVCount].mX=aRealVertex.x;
						aTriangle->mVertex[aVCount].mY=aRealVertex.z;
						aTriangle->mVertex[aVCount].mZ=aRealVertex.y;

						float aVecLength=sqrt(aRealVertexNormal.x*aRealVertexNormal.x+aRealVertexNormal.y*aRealVertexNormal.y+aRealVertexNormal.z*aRealVertexNormal.z);
						aRealVertexNormal.x/=aVecLength;
						aRealVertexNormal.y/=aVecLength;
						aRealVertexNormal.z/=aVecLength;

						aTriangle->mVertex[aVCount].mNormalX=aRealVertexNormal.x;
						aTriangle->mVertex[aVCount].mNormalY=aRealVertexNormal.z;
						aTriangle->mVertex[aVCount].mNormalZ=aRealVertexNormal.y;


/*
						aTriangle->mVertex[aVCount].mX=aRealVertex.x;
						aTriangle->mVertex[aVCount].mY=aRealVertex.z;
						aTriangle->mVertex[aVCount].mZ=aRealVertex.y;

						aTriangle->mVertex[aVCount].mNormalX=aRealVertexNormal.x;
						aTriangle->mVertex[aVCount].mNormalY=aRealVertexNormal.y;
						aTriangle->mVertex[aVCount].mNormalZ=aRealVertexNormal.z;
*/

						aTriangle->mVertex[aVCount].mTextureU=aTextureVertex.x;
						aTriangle->mVertex[aVCount].mTextureV=1.0-aTextureVertex.y;

						mExporter->mCube.Fit(aTriangle->mVertex[aVCount].mX,aTriangle->mVertex[aVCount].mY,aTriangle->mVertex[aVCount].mZ);
					}

					aTriangleList+=aTriangle;

				}

				// Now we can add all the triangles to the stripper...


				MeshStripper aStripper(aMesh.numFaces);
				{
				EnumPointerlist(Triangle,aTriangle,aTriangleList)
				{
					aStripper.AddVertex(&aTriangle->mVertex[0]);
					aStripper.AddVertex(&aTriangle->mVertex[1]);
					aStripper.AddVertex(&aTriangle->mVertex[2]);
				}
				}
				{
				EnumPointerlist(Triangle,aTriangle,aTriangleList)
				{
					aStripper.AddTriangle(aTriangle);
				}
				}
				if (aStripper.Go()==-1)
				{
					MessageBox(0,"Failed to organize into strips!","Hey",MB_OK);
				}
				else
				{
					//
					// Increment the renderable object count...
					//
					mExporter->mRenderableObjects++;

					//
					// Write out the name of this 'piece'
					//
					mExporter->WriteString(aObjectName);

					//
					// Write the pivot point...
					//
					mExporter->WriteFloat(aPivotPoint.x);
					mExporter->WriteFloat(aPivotPoint.y);
					mExporter->WriteFloat(aPivotPoint.z);

					//
					// Write the material...
					//
					if (aObjectMaterial!=NULL)
					{
						//
						// Get the material's transparency, so that will be our alpha info
						//
						float aTransparency=1.0f-aObjectMaterial->GetXParency();

						//
						// Get the material colors...
						//
						Color aAmbientColor=aObjectMaterial->GetAmbient();
						Color aDiffuseColor=aObjectMaterial->GetDiffuse();
						Color aSpecularColor=aObjectMaterial->GetSpecular();
						Color aEmissiveColor=aObjectMaterial->GetSelfIllumColor();

						//
						// Multiply specular lighting by the shine strength
						//
						float aShineStrength=aObjectMaterial->GetShinStr();
						aSpecularColor.r*=aShineStrength;
						aSpecularColor.g*=aShineStrength;
						aSpecularColor.b*=aShineStrength;

						//
						// Power of the specular lighting is the shininess * 100
						//
						float aPower=aObjectMaterial->GetShininess()*100;

						//
						// If we have any reflection map, we'll indicate it with a "1"
						//

						int aReflection=0;
						if (((StdMat*)aObjectMaterial)->MapEnabled(ID_RL)) aReflection=1;

						//
						// And we get the texture name
						//
						int aTexture=0;
						char aTextureFilename[256];
						if (((StdMat*)aObjectMaterial)->MapEnabled(ID_DI)) 
						{
							Texmap *aSubTexture=aObjectMaterial->GetSubTexmap(ID_DI);
							if (aSubTexture->ClassID()==Class_ID(BMTEX_CLASS_ID,0x00))
							{
								aTexture=1;
								char *aTName=((BitmapTex*)aSubTexture)->GetMapName();

								char aDriveLetter[256];
								char aDirectory[256];
								char aFilename[256];
								char aExtension[256];

								_splitpath(aTName,aDriveLetter,aDirectory,aFilename,aExtension);
								sprintf(aTextureFilename,"%s%s",aFilename,aExtension);
							}
						}



						mExporter->WriteFloat(aAmbientColor.r);
						mExporter->WriteFloat(aAmbientColor.g);
						mExporter->WriteFloat(aAmbientColor.b);
						mExporter->WriteFloat(aTransparency);

						mExporter->WriteFloat(aDiffuseColor.r);
						mExporter->WriteFloat(aDiffuseColor.g);
						mExporter->WriteFloat(aDiffuseColor.b);
						mExporter->WriteFloat(aTransparency);

						mExporter->WriteFloat(aSpecularColor.r);
						mExporter->WriteFloat(aSpecularColor.g);
						mExporter->WriteFloat(aSpecularColor.b);
						mExporter->WriteFloat(aTransparency);

						mExporter->WriteFloat(aEmissiveColor.r);
						mExporter->WriteFloat(aEmissiveColor.g);
						mExporter->WriteFloat(aEmissiveColor.b);
						mExporter->WriteFloat(aTransparency);

						mExporter->WriteFloat(aPower);
						mExporter->WriteInt(aReflection);
						mExporter->WriteInt(aTexture);
						if (aTexture) mExporter->WriteString(aTextureFilename);
					}
					else
					{
						//
						// No texture assigned...
						// So just make it grey
						//

						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(1.0f);

						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(1.0f);

						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(1.0f);

						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(.5f);
						mExporter->WriteFloat(1.0f);

						mExporter->WriteFloat(0.0f);
						mExporter->WriteInt(0);
						mExporter->WriteInt(0);
					}

					//
					// Write the triangle strips
					//

					int aStripCount=aStripper.mStrips.GetCount();
					mExporter->WriteInt(aStripCount);

					{
					EnumPointerlist(TriangleStrip,aStrip,aStripper.mStrips)
					{
						mExporter->WriteInt(aStrip->mTriangles);
						mExporter->WriteMemory(aStrip->mVertices,sizeof(struct Vertex)*(aStrip->mTriangles+2));
					}
					}
				}

				{
				EnumPointerlist(Triangle,aTriangle,aTriangleList)
				{
					delete aTriangle;
				}
				}

			}

		}
	}

	return TREE_CONTINUE;
}


