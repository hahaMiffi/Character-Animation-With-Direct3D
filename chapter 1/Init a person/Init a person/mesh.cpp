#include "mesh.h"

#pragma warning(disable:4996)

extern IDirect3DDevice9 *g_pDevice;

#define PATH_TO_TEXTURES "resources/meshes/"

Mesh::Mesh()
{
	m_pMesh = NULL;
}

Mesh::Mesh( char fName[] )
{
	m_pMesh = NULL;
	Load( fName );
}

Mesh::~Mesh()
{
	Release();
}

HRESULT Mesh::Load( char fName[] )
{
	Release();

	m_white.Ambient = m_white.Specular = m_white.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	m_white.Emissive = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );
	m_white.Power = 1.0f;

	ID3DXBuffer* adjacencyBfr = NULL;
	ID3DXBuffer* materialBfr = NULL;
	DWORD noMaterials = NULL;

	if( FAILED( D3DXLoadMeshFromX( fName, D3DXMESH_MANAGED, g_pDevice, &adjacencyBfr, &materialBfr, NULL, &noMaterials, &m_pMesh ) ) )
		return E_FAIL;

	D3DXMATERIAL* mtrls = (D3DXMATERIAL*)materialBfr->GetBufferPointer();

	for( int i = 0; i < (int)noMaterials; i++ )
	{
		m_materials.push_back( mtrls[i].MatD3D );

		if( mtrls[i].pTextureFilename != NULL )
		{
			char textureFileName[90];
			strcpy( textureFileName, PATH_TO_TEXTURES );
			strcat( textureFileName, mtrls[i].pTextureFilename );
			IDirect3DTexture9* tempTex = NULL;
			D3DXCreateTextureFromFile( g_pDevice, textureFileName, &tempTex );
			m_textures.push_back( tempTex );
		}
		else
			m_textures.push_back( NULL );
	}

	m_pMesh->OptimizeInplace( D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
							(DWORD*)adjacencyBfr->GetBufferPointer(), 0, 0, 0 );

	adjacencyBfr->Release();
	materialBfr->Release();

	return S_OK;
}

void Mesh::Render()
{
	int numMaterials = (int)m_materials.size();

	for( int i = 0; i < numMaterials; i++ )
	{
		if( m_textures[i] != NULL )
			g_pDevice->SetMaterial( &m_white );
		else
			g_pDevice->SetMaterial( &m_materials[i] );

		g_pDevice->SetTexture( 0, m_textures[i] );
		m_pMesh->DrawSubset(i);
	}
}

void Mesh::Release()
{
	if( m_pMesh != NULL )
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	int numTextures = (int)m_textures.size();

	for( int i = 0; i < numTextures; i++ )
		if( m_textures[i] != NULL )
			m_textures[i]->Release();

	m_textures.clear();
	m_materials.clear();
}