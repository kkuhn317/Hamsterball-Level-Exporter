// NodeEnumerator.cpp: implementation of the NodeEnumerator class.
//
//////////////////////////////////////////////////////////////////////

#include "NodeEnumerator.h"
#include "meshworldexport.h"
#include <meshadj.h>
#include <stdmat.h>
#include "meshstripper.h"
#include "worldmesh.h"
#include "light.h"
#include <modstack.h>

#include <math.h>
#include <plugapi.h>
#include <POLYSHP.H>
#include <linshape.h>

#include "console.h"
extern Console gOut;
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
	/*

	Okay, this chunk of code let's you see if something is a reference...
	INCLUDING with groups.  So, if an item is a reference, and points back
	to a group with the head name "INSTANCE:" then you wouldn't compile it in

	Object *aInstancePointer = node->GetObjectRef()->FindBaseObject();
	if (aInstancePointer!=node->GetObjectRef()) gOut.Out("INSTANCE! %s",node->GetName());
	else gOut.Out("Not Instance");
	*/

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

		char *aTheObjectName=node->GetName();
		ObjectState aObjectState=node->EvalWorldState(0);


		if (aObjectState.obj->SuperClassID()==HELPER_CLASS_ID || strnicmp(aTheObjectName,"REF:",4)==0)
		{

			Object *aObject=node->EvalWorldState(0).obj;
			ObjectState aObjectState=node->EvalWorldState(0);

			if (aObjectState.obj->ClassID()==Class_ID(POINTHELP_CLASS_ID,0) || strnicmp(aTheObjectName,"REF:",4)==0)
			{
				//
				// It's a point...`
				// So, we just need the XYZ and name, right?
				//

				Matrix3 aTargetMatrix;
				node->GetTargetTM(0,aTargetMatrix);
				Point3 aTargetPoint=aTargetMatrix.GetTrans();

				//
				// Hey, are you having trouble with the pivot point of an item?
				// Make a box, and put it at 0,0,0.  Then attach your item to it.
				// Now delete the box.  That fixes it.
				//

				Matrix3 aWorldMatrix=node->GetObjectTM(0);
//				Matrix3 aWorldMatrix=node->GetNodeTM(0);
				Point3 aWorldPoint=aWorldMatrix.GetTrans();

				//FUNK

				float aYaw;
				float aPitch;
				float aRoll;

				aWorldMatrix.GetYawPitchRoll(&aYaw,&aPitch,&aRoll);

				aYaw=(aYaw*180)/gMath.xPI;
				aPitch=(aPitch*180)/gMath.xPI;
				aRoll=(aRoll*180)/gMath.xPI;

				xPoint *aXPoint=new xPoint;

				aXPoint->mHasMaterial=0;

				char aTempName[256];
				strcpy(aTempName,node->GetName());

				if (strnicmp(aTheObjectName,"REF:",4)==0) strcpy(aXPoint->mName,&aTempName[4]);
				else strcpy(aXPoint->mName,node->GetName());
				aXPoint->mPosition.x=aWorldPoint.x;
				aXPoint->mPosition.y=aWorldPoint.y;
				aXPoint->mPosition.z=aWorldPoint.z;

				aXPoint->mRotation.x=aYaw;
				aXPoint->mRotation.y=aPitch;
				aXPoint->mRotation.z=aRoll;

				gOut.Out("REF POINT [%s]: %f %f %f - %f %f %f",
					aTheObjectName,
					aXPoint->mPosition.x,
					aXPoint->mPosition.y,
					aXPoint->mPosition.z,
					aXPoint->mRotation.x,
					aXPoint->mRotation.y,
					aXPoint->mRotation.z);

				//aXPoint->mHasMaterial=0;

				if (strnicmp(aTheObjectName,"REF:",4)==0)
				{
					Mtl *aObjectMaterial=node->GetMtl();
					if (aObjectMaterial!=NULL)
					{

						if (aObjectMaterial->ClassID()==Class_ID(DMTL_CLASS_ID,0))
						{
gOut.Out("Reference w/Material");

							aXPoint->mHasMaterial=1;
							float aTransparency=1.0f-aObjectMaterial->GetXParency();

							Color aAmbientColor=aObjectMaterial->GetAmbient();
							Color aDiffuseColor=aObjectMaterial->GetDiffuse();
							Color aSpecularColor=aObjectMaterial->GetSpecular();
							Color aEmissiveColor=aObjectMaterial->GetSelfIllumColor();

							float aShineStrength=aObjectMaterial->GetShinStr();
							aSpecularColor.r*=aShineStrength;
							aSpecularColor.g*=aShineStrength;
							aSpecularColor.b*=aShineStrength;

							float aPower=aObjectMaterial->GetShininess()*100;
							if (((StdMat*)aObjectMaterial)->MapEnabled(ID_DI)) 
							{
								Texmap *aSubTexture=aObjectMaterial->GetSubTexmap(ID_DI);
								if (aSubTexture->ClassID()==Class_ID(BMTEX_CLASS_ID,0x00))
								{
									char *aTName=((BitmapTex*)aSubTexture)->GetMapName();
		
									char aDriveLetter[256];
									char aDirectory[256];
									char aFilename[256];
									char aExtension[256];

									char aTexName[256];
		
									_splitpath(aTName,aDriveLetter,aDirectory,aFilename,aExtension);
									sprintf(aTexName,"%s%s",aFilename,aExtension);
									//aXPoint->mMaterial.mTexture=aXPoint->mTextureName;

									aXPoint->mMaterial.mTexture=(char*)malloc(strlen(aTexName)+1);
									strcpy(aXPoint->mMaterial.mTexture,aTexName);
								}
							}

							aXPoint->mMaterial.SetDiffuse(aDiffuseColor.r,aDiffuseColor.g,aDiffuseColor.b,aTransparency);
							aXPoint->mMaterial.SetAmbient(aAmbientColor.r,aAmbientColor.g,aAmbientColor.b,aTransparency);
							aXPoint->mMaterial.SetSpecular(aSpecularColor.r,aSpecularColor.g,aSpecularColor.b,aTransparency);
							aXPoint->mMaterial.SetEmissive(aEmissiveColor.r,aEmissiveColor.g,aEmissiveColor.b,aTransparency);
							aXPoint->mMaterial.mMaterial.Power=aPower;

						}
					}
				}



				mExporter->mPoints+=aXPoint;

				return TREE_CONTINUE;
			}
		}




		if (node->Renderable())
		{

			Object *aObject=node->EvalWorldState(0).obj;


			ObjectState aObjectState=node->EvalWorldState(0);

			if (aObjectState.obj->SuperClassID()==LIGHT_CLASS_ID)
			{
				//
				// It's a light!
				//
				// Look for: TDIR_LIGHT_CLASS_ID at the moment...
				// 
				// We need to store that as a light, position, target, color, etc, etc.

				if (aObjectState.obj->ClassID()==Class_ID(TDIR_LIGHT_CLASS_ID,0))
				{
					if (aObject->CanConvertToType(Class_ID(TDIR_LIGHT_CLASS_ID,0)))
					{
						Matrix3 aTargetMatrix;
						node->GetTargetTM(0,aTargetMatrix);
						Point3 aTargetPoint=aTargetMatrix.GetTrans();

						Matrix3 aWorldMatrix=node->GetObjectTM(0);
						Point3 aWorldPoint=aWorldMatrix.GetTrans();

						LightObject *aLight=(LightObject*)aObject->ConvertToType(0,Class_ID(TDIR_LIGHT_CLASS_ID,0));

						Point3 aLightColor=aLight->GetRGBColor(0,FOREVER);

						xLight *aXLight=new xLight();
						aXLight->mColor.x=aLightColor.x;
						aXLight->mColor.y=aLightColor.y;
						aXLight->mColor.z=aLightColor.z;

						aXLight->mPosition.x=aWorldPoint.x;
						aXLight->mPosition.y=aWorldPoint.y;
						aXLight->mPosition.z=aWorldPoint.z;

						aXLight->mLookat.x=aTargetPoint.x;
						aXLight->mLookat.y=aTargetPoint.y;
						aXLight->mLookat.z=aTargetPoint.z;

						aXLight->mType=xLight::DISTANTLIGHT;

						mExporter->mLights+=aXLight;


						// Okay, have light position, and light point...
						// Now what?
					}

					// Target Directional Light!
					// Get the info, and save it in the exporter's light list.
				}


				return TREE_CONTINUE;
			}

			if (aObject->CanConvertToType(Class_ID(LINEARSHAPE_CLASS_ID,0)))
			{
				//
				// It's a spline, which means we save this as a buncha lines.
				// First, get a usable object...
				//
				LinearShape *aLine=(LinearShape*)aObject->ConvertToType(0,Class_ID(LINEARSHAPE_CLASS_ID,0));
				PolyShape aShape=aLine->GetShape();

				Matrix3 aWorldMatrix=node->GetObjectTM(0);

				//
				// Make a spline object in our own
				// format...
				//
				xSpline *aMySpline=new xSpline;

				strcpy(aMySpline->mName,node->GetName());

				//
				// Add all thepoints to our spline.  A spline is just
				// a path of points, all other data must be extrapolated.
				//
				for (int aPCount=0;aPCount<aShape.numLines;aPCount++)
				{
					PolyLine aPLine=aShape.lines[aPCount];

					//
					// Access to points...
					//
					for (int aCount=0;aCount<aPLine.numPts;aCount++)
					{	
						PolyPt aPoint=aPLine.pts[aCount];
						Point3 aVertex(aPoint.p.x,aPoint.p.y,aPoint.p.z);
						Point3 aRealVertex=aWorldMatrix.PointTransform(aVertex);

						Point3 *aSplinePoint=new Point3;
						aSplinePoint->x=aRealVertex.x;
						aSplinePoint->y=aRealVertex.y;
						aSplinePoint->z=aRealVertex.z;

						aMySpline->mPointList+=aSplinePoint;
					}
				}

				//
				// So, now stash a line object.
				//
				mExporter->mSplines+=aMySpline;

			}
			else if (aObject->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID,0)))
			{
				//
				// Make a geometryobject and populate the material...
				//

				GeometryObject *aGeom=new GeometryObject();
				mExporter->mMesh->mMeshData->mGeoms+=aGeom;

				char *aObjectName=node->GetName();
				Mtl *aObjectMaterial=node->GetMtl();


				aGeom->mName=(char*)malloc(strlen(aObjectName)+1);
				strcpy(aGeom->mName,aObjectName);

				if (aObjectMaterial!=NULL)
				{
					Mtl *aTestMTL=node->GetMtl();
					if (aTestMTL->AnyMulti())
					{
						char xMsg[256];
						sprintf(xMsg,"Node [%s] has Multi Material!",node->GetName());
						MessageBox(0,xMsg,"CANNOT EXPORT!",MB_OK);
						return TREE_CONTINUE;
					}

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

					aGeom->mMaterialRef.SetAmbient(aAmbientColor.r,aAmbientColor.g,aAmbientColor.b,aTransparency);
					aGeom->mMaterialRef.SetDiffuse(aDiffuseColor.r,aDiffuseColor.g,aDiffuseColor.b,aTransparency);
					aGeom->mMaterialRef.SetSpecular(aSpecularColor.r,aSpecularColor.g,aSpecularColor.b,aTransparency*aShineStrength);
					aGeom->mMaterialRef.SetEmissive(aEmissiveColor.r,aEmissiveColor.g,aEmissiveColor.b,aTransparency);
					aGeom->mMaterialRef.mMaterial.Power=aPower;

					aGeom->mMaterialRef.mHasReflection=(aReflection!=0);
					if (aTexture) 
					{
						aGeom->mMaterialRef.mTexture=(char*)malloc(strlen(aTextureFilename)+1);
						strcpy(aGeom->mMaterialRef.mTexture,aTextureFilename);
					}
				}
				else
				{
					aGeom->mMaterialRef.SetAmbient(.5,.5,.5,1);
					aGeom->mMaterialRef.SetDiffuse(.5,.5,.5,1);
					aGeom->mMaterialRef.SetSpecular(.5,.5,.5,1);
					aGeom->mMaterialRef.SetEmissive(.5,.5,.5,1);
				}


				//
				// Now add all the triangles to the faces list...
				//

				TriObject *aTriObject=(TriObject*)aObject->ConvertToType(0,Class_ID(TRIOBJ_CLASS_ID,0));
				Mesh aMesh=aTriObject->GetMesh();
				aMesh.RemoveDegenerateFaces();
			
				aMesh.buildNormals();

				int aTextureVertices=aMesh.getNumTVerts();


				// Prepare a triangle stripper.
				// We want to dump all the faces from this object
				// into the stripper for generation of triangle
				// strips.

				Matrix3 aPivotMatrix=node->GetNodeTM(0);
				Matrix3 aWorldMatrix=node->GetObjectTM(0);

				Point3 aPivotPoint=aPivotMatrix.GetTrans();
				Point3 aWorldPoint=aWorldMatrix.GetTrans();

				for (int aFaceCount=0;aFaceCount<aMesh.numFaces;aFaceCount++)
				{
					Face aFace=aMesh.faces[aFaceCount];
					TVFace aTexFace;

					if (aTextureVertices>0) aTexFace=aMesh.tvFace[aFaceCount];


					Triangle *aTriangle=new Triangle;
					aGeom->mFaces+=aTriangle;


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

						D3DXVECTOR3 aVec,aNewVec;
						aVec.x=aRealVertexNormal.x;
						aVec.y=aRealVertexNormal.y;
						aVec.z=aRealVertexNormal.z;

						D3DXVec3Normalize(&aNewVec,&aVec);

						aTriangle->mVertex[aVCount].mNormalX=aNewVec.x;
						aTriangle->mVertex[aVCount].mNormalY=aNewVec.z;
						aTriangle->mVertex[aVCount].mNormalZ=aNewVec.y;

//						if (fabs(aRealVertexNormal.x)>1 || fabs(aRealVertexNormal.y)>1 || fabs(aRealVertexNormal.z)>1) gOut.Out("Uh... fucky?");



						aTriangle->mVertex[aVCount].mTextureU=aTextureVertex.x;
						aTriangle->mVertex[aVCount].mTextureV=1.0-aTextureVertex.y;

						//aGeom->mFaces.Add(aTriangle);
						mExporter->mMesh->mMeshData->mCube.Fit(aTriangle->mVertex[aVCount].mX,aTriangle->mVertex[aVCount].mY,aTriangle->mVertex[aVCount].mZ);
					}
				}
			}
		}
	}

	return TREE_CONTINUE;
}


