//
// Class to decode 3DS Max ASE files
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2003
//

#include	<stdio.h>

#include	"AseDecoder.h"
#include	"Data.h"
#include	"ObjStr.h"
#include	"Dictionary.h"
#include	"Mesh3D.h"
#include	"MeshObject.h"
#include	"CompositeObject.h"
#include	"Vector2D.h"
#include	"Vector3D.h"
#include	"Vector4D.h"
#include	"ResourceManager.h"
#include	"Texture.h"
#include	"log.h"

MetaClass	AseDecoder :: classInstance ( "AseDecoder", &ResourceDecoder :: classInstance );

bool	AseDecoder :: checkExtension ( const String& theName )
{
	return String :: getExtension ( theName ).toLower () == "ase";
}

Object * AseDecoder :: decode ( Data * data )
{
	if ( !data -> isAscii () )
		return NULL;

	Dictionary	materials;
	String	str;
	String	cmd, args;
	String	materialBitmap;
	String	materialNo;
	String	currentMaterial;
	String	nodeName;
	bool	ok  = true;

	CompositeObject * res = new CompositeObject ( data -> getName (), Vector3D ( 0, 0, 0 ) );

	res -> init ();

	int		numVertices    = 0;
	int		numFaces       = 0;
	int		numTexVertices = 0;
	Vector2D texTile ( 1, 1 );
	Vector2D texOffs ( 0, 0 );

	Vector3D * vertices  = NULL;
	Vector3D * normals   = NULL;
	Vector2D * texCoords = NULL;

	Mesh3D :: Face * faces = NULL;

	while ( data -> getString ( str ) )
	{
		str.trim ();

		String :: parseString ( str, cmd, args );

		if ( cmd == "*MESH" )				// start new mesh
		{
											// save current mesh (if any)
			if ( numVertices < 1 || numFaces < 1 || numTexVertices < 1 )
				continue;
											// transform texture coords
			for ( int i = 0; i < numTexVertices; i++ )
				texCoords [i] = texOffs + texCoords [i] * texTile;

											// create triangular mesh object
			Mesh3D * triMesh = new Mesh3D ( "mesh", vertices, normals, numVertices, texCoords, numTexVertices,
										    faces, numFaces, NULL );

			triMesh -> computeNormals ();	// compute normals for it's vertices

											// now check for material
			String * value = dynamic_cast <String *> ( materials.itemForKey ( currentMaterial ) );

			if ( value != NULL )			// material definition present
			{
				Texture * texture = getResourceManager () -> getTexture ( *value );

				if ( texture != NULL )
				{
					triMesh -> setTexture ( texture );
					texture -> release ();
				}
				else
				{
//					ok = false;

					(*sysLog) << "AseDecoder: error loding texture " << *value << logEndl;
				}
			}

											// now add to composite object
			MeshObject * mesh = new MeshObject ( nodeName, Vector3D ( 0, 0, 0 ), Vector4D ( 1, 1, 1, 1 ), 
											     Transform3D :: getIdentity (), triMesh );

			mesh -> init ();
			res  -> insert ( mesh );

											// reset state for next mesh (if any)
			delete [] vertices;
			delete [] texCoords;
			delete normals;

			numVertices    = 0;
			numFaces       = 0;
			numTexVertices = 0;
			vertices       = NULL;
			texCoords      = NULL;
			normals        = NULL;
		}
		else
		if ( cmd == "*MATERIAL_REF" )		// link current mesh with material
		{
			currentMaterial = args;
		}
		else
		if ( cmd == "*NODE_NAME" )
			nodeName = args;
		else
		if ( cmd == "*MESH_NUMVERTEX" )
		{
			numVertices = args.toInt ();

			if ( numVertices < 1 )
			{
				ok = false;

				break;
			}

			vertices  = new Vector3D [numVertices];
		}
		else
		if ( cmd == "*MESH_NUMFACES" )
		{
			numFaces = args.toInt ();

			if ( numFaces < 1 )
			{
				ok = false;

				break;
			}

			faces = new Mesh3D :: Face [numFaces];
		}
		else
		if ( cmd == "*MESH_NUMTVERTEX" )
		{
			numTexVertices = args.toInt ();

			if ( numTexVertices < 0 )
			{
				ok = false;

				break;
			}

			texCoords = new Vector2D [numTexVertices];
		}
		else
		if ( cmd == "*MESH_VERTEX" )
		{
			int			index;
			Vector3D	v;

			if ( sscanf_s ( args, "%d %f %f %f", &index, &v.x, &v.y, &v.z ) != 4 )
			{
				ok = false;

				break;
			}

			if ( index < 0 || index >= numVertices )
			{
				ok = false;

				break;
			}

			vertices [index] = v;
		}
		else
		if ( cmd == "*MESH_FACE" )
		{
			int	index;
			int	i1, i2, i3;

			if ( sscanf_s ( args, "%d: \tA:\t%d B:\t%d C:\t%d", &index, &i1, &i2, &i3 ) != 4 )
			{
				ok = false;

				break;
			}

			if ( index < 0 || index >= numFaces )
			{
				ok = false;

				break;
			}

			faces [index].vertexIndex [0] = i1;
			faces [index].vertexIndex [1] = i2;
			faces [index].vertexIndex [2] = i3;
		}
		else
		if ( cmd == "*MESH_TVERT" )
		{
			int			index;
			Vector2D	uv;

			if ( sscanf_s ( args, "%d %f %f", &index, &uv.x, &uv.y ) != 3 )
			{
				ok = false;

				break;
			}

			if ( index < 0 || index > numTexVertices )
			{
				ok = false;

				break;
			}

			texCoords [index] = uv;
		}
		else
		if ( cmd == "*MESH_TFACE" )
		{
			int	index;
			int	i1, i2, i3;

			if ( sscanf_s ( args, "%i %i %i %i", &index, &i1, &i2, &i3 ) != 4 )
			{
				ok = false;

				break;
			}

			if ( index < 0 || index >= numFaces )
			{
				ok = false;

				break;
			}

			faces [index].texCoordIndex [0] = i1;
			faces [index].texCoordIndex [1] = i2;
			faces [index].texCoordIndex [2] = i3;
		}
		else
		if ( cmd == "*MATERIAL" )			// materials section, extract number and texture name
			materialNo = String ( args.toInt () );
		else
		if ( cmd == "*UVW_U_TILING" )
			texTile.x = args.toFloat ();
		else
		if ( cmd == "*UVW_V_TILING" )
			texTile.y = args.toFloat ();
		else
		if ( cmd == "*UVW_U_OFFSET" )
			texOffs.x = args.toFloat ();
		else
		if ( cmd == "*UVW_V_OFFSET" )
			texOffs.y = args.toFloat ();
		else
		if ( cmd == "*BITMAP" )
		{
			materialBitmap = args.dequote ();

											// store material ref
			String * key   = new String ( materialNo );
			String * value = new String ( materialBitmap );

			materials.insert ( key, value );

											// since they are retain'ed in insert operation
			key   -> release ();
			value -> release ();
		}
	}

											// save current mesh (if any)
	if ( ok && numVertices > 0 && numFaces > 0 && numTexVertices > 0 )
	{
											// transform texture coords
		for ( int i = 0; i < numTexVertices; i++ )
			texCoords [i] = texOffs + texCoords [i] * texTile;

											// create triangular mesh object
		Mesh3D * triMesh = new Mesh3D ( "mesh", vertices, normals, numVertices, texCoords, numTexVertices,
									    faces, numFaces, NULL );

		triMesh -> computeNormals ();		// compute normals for it's vertices

											// now check for material
		String * value = dynamic_cast <String *> ( materials.itemForKey ( currentMaterial ) );

		if ( value != NULL )				// material definition present
		{
			Texture * texture = getResourceManager () -> getTexture ( *value );

			if ( texture != NULL )
			{
				triMesh -> setTexture ( texture );
				texture -> release ();
			}
			else
			{
//				ok = false;

				(*sysLog) << "AseDecoder: error loding texture " << *value << logEndl;
			}
		}

											// now add to composite object
		MeshObject * mesh = new MeshObject ( nodeName, Vector3D ( 0, 0, 0 ), Vector4D ( 1, 1, 1, 1 ), 
										     Transform3D :: getIdentity (), triMesh );

		mesh -> init ();
		res  -> insert ( mesh );
	}

											// free allocated structs
	delete [] vertices;
	delete [] texCoords;
	delete normals;
	delete [] faces;

	if ( !ok )
	{
		delete res;

		return NULL;
	}

	return res;
}
