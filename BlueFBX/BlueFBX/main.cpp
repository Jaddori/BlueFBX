#if WIN32
#include "fbxsdk.h"
#else
#include <fbxsdk.h>
#endif

#include <vector>
#include <cassert>
using namespace std;

struct Vertex
{
	float position[3];
	float uv[2];
	float normal[3];
	float tangent[3];
	float bitangent[3];
};

vector<Vertex> vertexData;

void processVertices( FbxMesh* mesh )
{
	FbxVector4* vertices = mesh->GetControlPoints();

	const int TRIANGLE_VERTEX_COUNT = 3;
	for( int i=0; i<mesh->GetPolygonCount(); i++ )
	{
		for( int j=0; j<TRIANGLE_VERTEX_COUNT; j++ )
		{
			Vertex vertex;

			int index = mesh->GetPolygonVertex( i, j );

			vertex.position[0] = (float)vertices[index].mData[0];
			vertex.position[1] = (float)vertices[index].mData[1];
			vertex.position[2] = (float)vertices[index].mData[2];

			vertexData.push_back( vertex );
		}
	}
}

void processUV( FbxMesh* mesh )
{
	FbxStringList uvNameList;
	mesh->GetUVSetNames( uvNameList );

	for( int i=0; i<uvNameList.GetCount(); i++ )
	{
		const char* uvName = uvNameList.GetStringAt( i );
		const FbxGeometryElementUV* uvElement = mesh->GetElementUV( uvName );

		if( uvElement )
		{
			const bool useIndex = uvElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect;
			const int INDEX_COUNT = ( useIndex ? uvElement->GetIndexArray().GetCount() : 0 );
			const int POLY_COUNT = mesh->GetPolygonCount();

			if( uvElement->GetMappingMode() == FbxGeometryElement::eByControlPoint )
			{
				for( int j = 0; j<POLY_COUNT; j++ )
				{
					const int SIZE = mesh->GetPolygonSize( j );
					for( int k=0; k<SIZE; k++ )
					{
						FbxVector2 uv;
						int polyVertexIndex = mesh->GetPolygonVertex( j, k );
						int uvIndex = ( useIndex ? uvElement->GetIndexArray().GetAt( polyVertexIndex ) : polyVertexIndex );

						uv = uvElement->GetDirectArray().GetAt( uvIndex );

						vertexData[k].uv[0] = (float)uv.mData[0];
						vertexData[k].uv[1] = (float)uv.mData[1];
					}
				}
			}
			else if( uvElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex )
			{
				int polyIndexCount = 0;
				for( int j=0; j<POLY_COUNT; j++ )
				{
					const int SIZE = mesh->GetPolygonSize(j);
					for( int k=0; k<SIZE; k++ )
					{
						FbxVector2 uv;

						int uvIndex = ( useIndex ? uvElement->GetIndexArray().GetAt( polyIndexCount ) : polyIndexCount );

						uv = uvElement->GetDirectArray().GetAt( uvIndex );

						vertexData[polyIndexCount].uv[0] = (float)uv.mData[0];
						vertexData[polyIndexCount].uv[1] = (float)uv.mData[1];

						polyIndexCount++;
					}
				}
			}
		}
	}
}

void processNormals( FbxMesh* mesh )
{
	FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();

	if( normalElement )
	{
		if( normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint )
		{
			for( int i=0; i<mesh->GetControlPointsCount(); i++ )
			{
				int index = 0;
				if( normalElement->GetReferenceMode() == FbxGeometryElement::eDirect )
				{
					index = i;
				}
				else if( normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect )
				{
					index = normalElement->GetIndexArray().GetAt(0);
				}

				FbxVector4 normals = normalElement->GetDirectArray().GetAt(index);

				vertexData[i].normal[0] = (float)normals.mData[0];
				vertexData[i].normal[1] = (float)normals.mData[1];
				vertexData[i].normal[2] = (float)normals.mData[2];
			}
		}
		else if( normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex )
		{
			int index = 0;

			for( int i=0; i<mesh->GetPolygonCount(); i++ )
			{
				int size = mesh->GetPolygonSize(i);

				for( int j=0; j<size; j++ )
				{
					int normalIndex = 0;

					if( normalElement->GetReferenceMode() == FbxGeometryElement::eDirect )
					{
						normalIndex = index;
					}
					else if( normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect )
					{
						normalIndex = normalElement->GetIndexArray().GetAt(index);
					}

					FbxVector4 normals = normalElement->GetDirectArray().GetAt(normalIndex);

					vertexData[index].normal[0] = (float)normals.mData[0];
					vertexData[index].normal[1] = (float)normals.mData[1];
					vertexData[index].normal[2] = (float)normals.mData[2];

					index++;
				}
			}
		}
	}
}

void processTangents( FbxMesh* mesh )
{
	int tangentCount = mesh->GetElementTangentCount();

	for( int i=0; i<tangentCount; i++ )
	{
		FbxGeometryElementTangent* tangentElement = mesh->GetElementTangent(i);

		if( tangentElement )
		{
			if( tangentElement->GetMappingMode() == FbxGeometryElement::eByControlPoint )
			{
				for( int j=0; j<mesh->GetControlPointsCount(); j++ )
				{
					int tangentIndex = 0;

					if( tangentElement->GetReferenceMode() == FbxGeometryElement::eDirect )
					{
						tangentIndex = j;
					}
					else if( tangentElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect )
					{
						tangentIndex = tangentElement->GetIndexArray().GetAt(0);
					}

					FbxVector4 tangents = tangentElement->GetDirectArray().GetAt( tangentIndex );

					vertexData[j].tangent[0] = (float)tangents.mData[0];
					vertexData[j].tangent[1] = (float)tangents.mData[1];
					vertexData[j].tangent[2] = (float)tangents.mData[2];
				}
			}
			else if( tangentElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex )
			{
				int index = 0;

				for( int j=0; j<mesh->GetPolygonCount(); j++ )
				{
					int size = mesh->GetPolygonSize( i );

					for( int k=0; k<size; k++ )
					{
						int tangentIndex = 0;

						if( tangentElement->GetReferenceMode() == FbxGeometryElement::eDirect )
						{
							tangentIndex = index;
						}
						else if( tangentElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect )
						{
							tangentIndex = tangentElement->GetIndexArray().GetAt( index );
						}

						FbxVector4 tangents = tangentElement->GetDirectArray().GetAt( tangentIndex );

						vertexData[index].tangent[0] = (float)tangents.mData[0];
						vertexData[index].tangent[1] = (float)tangents.mData[1];
						vertexData[index].tangent[2] = (float)tangents.mData[2];

						index++;
					}
				}
			}
		}
	}
}

void processBitangents( FbxMesh* mesh )
{
	int bitangentCount = mesh->GetElementBinormalCount();

	for( int i=0; i<bitangentCount; i++ )
	{
		FbxGeometryElementBinormal* biElement = mesh->GetElementBinormal( i );
		if( biElement )
		{
			if( biElement->GetMappingMode() == FbxGeometryElement::eByControlPoint )
			{
				for( int j=0; j<mesh->GetControlPointsCount(); j++ )
				{
					int biIndex = 0;

					if( biElement->GetReferenceMode() == FbxGeometryElement::eDirect )
					{
						biIndex = j;
					}
					else if( biElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect )
					{
						biIndex = biElement->GetIndexArray().GetAt(0);
					}

					FbxVector4 bitangents = biElement->GetDirectArray().GetAt( biIndex );

					vertexData[j].bitangent[0] = (float)bitangents.mData[0];
					vertexData[j].bitangent[1] = (float)bitangents.mData[1];
					vertexData[j].bitangent[2] = (float)bitangents.mData[2];
				}
			}
			else if( biElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex )
			{
				int index = 0;

				for( int j=0; j<mesh->GetPolygonCount(); j++ )
				{
					int size = mesh->GetPolygonSize(j);

					for( int k=0; k<size; k++ )
					{
						int biIndex = 0;

						if( biElement->GetReferenceMode() == FbxGeometryElement::eDirect )
						{
							biIndex = index;
						}
						else if( biElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect )
						{
							biIndex = biElement->GetIndexArray().GetAt( index );
						}

						FbxVector4 bitangents = biElement->GetDirectArray().GetAt( biIndex );

						vertexData[index].bitangent[0] = (float)bitangents.mData[0];
						vertexData[index].bitangent[1] = (float)bitangents.mData[1];
						vertexData[index].bitangent[2] = (float)bitangents.mData[2];

						index++;
					}
				}
			}
		}
	}
}

void processMesh( FbxMesh* mesh )
{
	processVertices( mesh );
	processUV( mesh );
	processNormals( mesh );
	processTangents( mesh );
	processBitangents( mesh );
}

void processNode( FbxNode* node )
{
	if( node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh )
	{
		FbxMesh* mesh = (FbxMesh*)node->GetNodeAttribute();
		processMesh( mesh );
	}
}

int main( int argc, char* argv[] )
{
	FbxManager* manager = FbxManager::Create();
	
	FbxIOSettings* ioSettings = FbxIOSettings::Create( manager, IOSROOT );
	manager->SetIOSettings( ioSettings );

	FbxImporter* importer = FbxImporter::Create( manager, "" );
	if( importer->Initialize( "./test.fbx", -1, manager->GetIOSettings() ) )
	{
		FbxScene* scene = FbxScene::Create( manager, "myScene" );
		importer->Import( scene );
		importer->Destroy();

		FbxNode* rootNode = scene->GetRootNode();
		if( rootNode )
		{
			for( int i=0; i<rootNode->GetChildCount(); i++ )
			{
				processNode( rootNode->GetChild(i) );
			}
		}

		manager->Destroy();

		FILE* outputFile = fopen( "./test.mesh", "wb" );
		if( outputFile )
		{
			int vertexCount = vertexData.size();
			fwrite( &vertexCount, sizeof(vertexCount), 1, outputFile );
			fwrite( &vertexCount, sizeof(vertexCount), 1, outputFile );
			fwrite( vertexData.data(), sizeof(Vertex), vertexData.size(), outputFile );

			vector<int> indices;
			for( int i=0; i<vertexCount; i++ )
			{
				indices.push_back(i);
			}
			fwrite( indices.data(), sizeof(int), indices.size(), outputFile );

			fclose( outputFile );
		}
	}
	else
	{
		printf( "Failed to initialize FbxImporter.\n" );
	}

	printf( "Done.\n" );

	int ex;
	//scanf_s( "%d", &ex );

	return 0;
}
