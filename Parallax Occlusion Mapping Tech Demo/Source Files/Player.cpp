#include "../Headers/Player.h"

bool Player::Load(LPCWSTR meshFile, LPCWSTR textureFile, LPCWSTR mapFile)
{
	armorBump = NULL;
	bodyBump = NULL;
	headBump = NULL;
	headgearBump = NULL;
	helmetBump = NULL;
	packBump = NULL;
	weaponsBump = NULL;

	// Create a place to store the effect
	m_pEffect = new Effect();

	// Load the shader effect file
	m_pEffect->Load(L"Shaders/BumpMapping.fx");

	
	// Massive Hack // shudder!
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		L"Armor_bumpmap.dds",
		&armorBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		L"Body_bumpmap.dds",
		&bodyBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		L"DwarfHead_bumpmap.dds",
		&headBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		L"Headgear_bumpmap.dds",
		&headgearBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		L"Helmet_bumpmap.dds",
		&helmetBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		L"Pack_bumpmap.dds",
		&packBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		L"Weapons_bumpmap.dds",
		&weaponsBump);

	textures[0] = weaponsBump;
	textures[1] = packBump;
	textures[2] = bodyBump;
	textures[3] = bodyBump;
	textures[4] = bodyBump;
	textures[5] = bodyBump;
	textures[6] = armorBump;
	textures[7] = headgearBump;
	textures[8] = headBump;
	// end of massive hack // Memory Leaks!!!!
	

	// Vertex Declaration // Easier than hacking tangents into FVF structures
	D3DVERTEXELEMENT9 decl[] = 
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
		{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};

	D3DObj::Instance()->GetDeviceClass()->CreateVertexDeclaration(decl, &m_pVertexDeclaration);

	// Create the material buffer for textures from the .x file
	LPD3DXBUFFER pD3DXMtrlBuffer;

	// Load the mesh from the specified file
	D3DXLoadMeshFromX( meshFile, D3DXMESH_SYSTEMMEM,
		D3DObj::Instance()->GetDeviceClass(), NULL,
		&pD3DXMtrlBuffer, NULL, &m_dwNumMaterials,
		&m_pMesh );

	LPD3DXMESH tempMesh;

	DWORD* rgdwAdjacency = NULL;
	rgdwAdjacency = new DWORD[ m_pMesh->GetNumFaces() * 3 ];

	m_pMesh->GenerateAdjacency( 1e-6f, rgdwAdjacency );

	D3DXCleanMesh(D3DXCLEAN_BOWTIES, m_pMesh, rgdwAdjacency, &tempMesh, rgdwAdjacency, NULL);

	m_pMesh = tempMesh;

	tempMesh->Release();

	LPD3DXMESH clonedMesh, newMesh;

	m_pMesh->CloneMesh(D3DXMESH_VB_MANAGED, decl, D3DObj::Instance()->GetDeviceClass(), &clonedMesh);

	m_pMesh->Release();

	D3DXComputeNormals(clonedMesh, rgdwAdjacency);

	D3DXComputeTangentFrameEx(clonedMesh, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
		D3DDECLUSAGE_BINORMAL, 0,
		D3DDECLUSAGE_NORMAL, 0, 0, rgdwAdjacency, -1.01f,
		-0.01f, -1.01f, &newMesh, NULL );

	clonedMesh->Release();

	m_pMesh = newMesh;
	newMesh = 0;

	// We need to extract the material properties and texture names from the
	// pD3DXMtrlBuffer
	D3DXMATERIAL* d3dxMaterials = ( D3DXMATERIAL* )pD3DXMtrlBuffer->GetBufferPointer();
	m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];

	m_pMeshTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];


	for( DWORD i = 0; i < m_dwNumMaterials; i++ )
	{
		// Copy the material
		m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

		m_pMeshTextures[i] = NULL;

		if( d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlenA( d3dxMaterials[i].pTextureFilename ) > 0 )
		{
			D3DXCreateTextureFromFileA( D3DObj::Instance()->GetDeviceClass(),
				d3dxMaterials[i].pTextureFilename,
				&m_pMeshTextures[i]);
		}
	}

	pD3DXMtrlBuffer->Release();

	return true;
}

void Player::Render()
{
	D3DXMatrixTranslation(&m_matTranslate, this->GetXPosition(), this->GetYPosition(), this->GetZPosition());
	D3DXMatrixScaling(&m_matScaling, this->GetXScale(), this->GetYScale(), this->GetZScale());
	D3DXMatrixRotationX(&m_matRotationX, this->GetXRotation());
	D3DXMatrixRotationY(&m_matRotationY, this->GetYRotation());
	D3DXMatrixRotationZ(&m_matRotationZ, this->GetZRotation());

	D3DXMATRIXA16 mWorld;
	D3DXMATRIXA16 mWVP;
	D3DXMATRIXA16 mWV;

	D3DXMatrixMultiply(&m_matRot,&m_matRotationX, &m_matRotationY); 
	D3DXMatrixMultiply(&m_matRot,&m_matRot, &m_matRotationZ);
	D3DXMatrixMultiply(&m_matRot,&m_matRot, &m_matScaling);

	D3DXMatrixMultiply(&mWorld, &m_matRot, &m_matTranslate);

	D3DObj::Instance()->GetDeviceClass()->SetTransform( D3DTS_WORLD, &mWorld);

	D3DObj::Instance()->GetDeviceClass()->SetVertexDeclaration(m_pVertexDeclaration);

	// select the vertex buffer to display
	D3DObj::Instance()->GetDeviceClass()->SetStreamSource(0, m_pVBuffer, 0, sizeof(CUSTOMVERTEX));

	D3DXMATRIXA16 mView;
	D3DXMATRIXA16 mProjection;

	mView = CameraObj::Instance()->GetViewMatrix();
	mProjection = CameraObj::Instance()->GetProjectionMatrix();

	D3DXMatrixMultiply(&mWV, &mWorld, &mView);
	D3DXMatrixMultiply(&mWVP, &mWV, &mProjection);

	D3DXMATRIX EyePoint = CameraObj::Instance()->GetEyePoint();

	m_pEffect->GetEffect()->SetValue("ViewVector", &EyePoint, sizeof(EyePoint));

	m_pEffect->GetEffect()->SetValue("World", &mWorld, sizeof(mWorld));
	m_pEffect->GetEffect()->SetValue("Projection", &mProjection, sizeof(mProjection));

	m_pEffect->GetEffect()->SetValue("View", &mView, sizeof(mView));

	// Get Inverse Transpose of World Matrix
	D3DXMATRIX m;
	D3DXMatrixInverse(&m, NULL, &mWorld);
	D3DXMATRIX j;
	D3DXMatrixTranspose(&j, &m );
	D3DXMATRIX inv = j;

	m_pEffect->GetEffect()->SetMatrix("WorldInverseTranspose", &inv);

	m_pEffect->GetEffect()->SetTechnique("BumpMapped");

	UINT iPass, cPasses;

	m_pEffect->GetEffect()->Begin(&cPasses,0);

	for(iPass = 0; iPass < cPasses; iPass++)
	{
		m_pEffect->GetEffect()->BeginPass(iPass);

		for (DWORD i=0; i < m_dwNumMaterials; i++)
		{
			// Set the material and texture for this subset
			D3DObj::Instance()->GetDeviceClass()->SetMaterial(&m_pMeshMaterials[i]);
			D3DObj::Instance()->GetDeviceClass()->SetTexture(0,m_pMeshTextures[i]);
			m_pEffect->GetEffect()->SetTexture("ModelTexture", m_pMeshTextures[i]);
			m_pEffect->GetEffect()->CommitChanges();
			m_pEffect->GetEffect()->SetTexture("NormalMap", textures[i]);
			m_pEffect->GetEffect()->CommitChanges();

			// Draw the mesh subset
			m_pMesh->DrawSubset( i );
		}

		m_pEffect->GetEffect()->EndPass();
	}
	m_pEffect->GetEffect()->End();
}

void Player::Update(float dt)
{
	GameObject::Update(dt);

}

void Player::Clean()
{
}