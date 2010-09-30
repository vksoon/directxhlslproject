#include "../Headers/Player.h"

bool Player::Load(File * pFile)
{
	std::string id;
	std::string meshFile, shaderConfigFile;
	float xrot,yrot,zrot,xpos,ypos,zpos,xscale,yscale,zscale, uniformscale;

	pFile->GetString(&id);
	pFile->GetString(&meshFile);
	pFile->GetString(&shaderConfigFile);
	pFile->GetFloat(&xrot);
	pFile->GetFloat(&yrot);
	pFile->GetFloat(&zrot);
	pFile->GetFloat(&xscale);
	pFile->GetFloat(&yscale);
	pFile->GetFloat(&zscale);
	pFile->GetFloat(&xpos);
	pFile->GetFloat(&ypos);
	pFile->GetFloat(&zpos);
	pFile->GetFloat(&uniformscale);

	SetXRotation(xrot);
	SetYRotation(yrot);
	SetZRotation(zrot);
	SetXPosition(xpos);
	SetYPosition(ypos);
	SetZPosition(zpos);
	SetXScale(xscale);
	SetYScale(yscale);
	SetZScale(zscale);
	SetOverallScale(uniformscale);

	m_id = id;

	armorBump = NULL;
	bodyBump = NULL;
	headBump = NULL;
	headgearBump = NULL;
	helmetBump = NULL;
	packBump = NULL;
	weaponsBump = NULL;

	// Create an Effect Factory and Register some Creator Objects
	Factory<Effect> EffectFactory;
	EffectFactory.Register("BumpMapEffect", new Creator<BumpMapEffect, Effect>);

	// Create the Lighting Effect file
	m_pBumpMapEffect = EffectFactory.Create("BumpMapEffect");
	assert(!_stricmp(m_pBumpMapEffect->GetClassName(), "BumpMapEffect")); // Check the class name exists

	m_pBumpMapEffect->Load("Shaders/BumpMapping.fx");
	m_pBumpMapEffect->LoadValuesFromConfig(shaderConfigFile);

	/* Adding bump map textures to .x didn't seem to be possible as it only supports
	   one texture per material, so I added an array of bump map textures that are
	   applied in the same order as the textures loaded from the .x file */
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		"Assets/Dwarf/Armor_bumpmap.dds",
		&armorBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		"Assets/Dwarf/Body_bumpmap.dds",
		&bodyBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		"Assets/Dwarf/DwarfHead_bumpmap.dds",
		&headBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		"Assets/Dwarf/Headgear_bumpmap.dds",
		&headgearBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		"Assets/Dwarf/Helmet_bumpmap.dds",
		&helmetBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		"Assets/Dwarf/Pack_bumpmap.dds",
		&packBump);
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		"Assets/Dwarf/Weapons_bumpmap.dds",
		&weaponsBump);

	m_bumpTextures[0] = weaponsBump;
	m_bumpTextures[1] = packBump;
	m_bumpTextures[2] = bodyBump;
	m_bumpTextures[3] = bodyBump;
	m_bumpTextures[4] = bodyBump;
	m_bumpTextures[5] = bodyBump;
	m_bumpTextures[6] = armorBump;
	m_bumpTextures[7] = headgearBump;
	m_bumpTextures[8] = headBump;
	

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
	if( FAILED(D3DXLoadMeshFromX( meshFile.c_str(), D3DXMESH_MANAGED,
		D3DObj::Instance()->GetDeviceClass(), NULL,
		&pD3DXMtrlBuffer, NULL, &m_dwNumMaterials,
		&m_pMesh )))
	{
		MessageBox(NULL, "Could not find player mesh file!", "Mesh Loading", MB_OK);
	}

	LPD3DXMESH tempMesh;

	DWORD* rgdwAdjacency = NULL;
	rgdwAdjacency = new DWORD[ m_pMesh->GetNumFaces() * 3];

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

void Player::Draw()
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

	m_pBumpMapEffect->Get()->SetValue("ViewVector", &EyePoint, sizeof(EyePoint));

	m_pBumpMapEffect->Get()->SetValue("World", &mWorld, sizeof(mWorld));
	m_pBumpMapEffect->Get()->SetValue("Projection", &mProjection, sizeof(mProjection));

	m_pBumpMapEffect->Get()->SetValue("View", &mView, sizeof(mView));

	// Get Inverse Transpose of World Matrix
	D3DXMATRIX m;
	D3DXMatrixInverse(&m, NULL, &mWorld);
	D3DXMATRIX j;
	D3DXMatrixTranspose(&j, &m );
	D3DXMATRIX inv = j;

	m_pBumpMapEffect->Get()->SetMatrix("WorldInverseTranspose", &inv);

	m_pBumpMapEffect->Get()->SetTechnique("BumpMapped");

	UINT iPass, cPasses;

	m_pBumpMapEffect->Get()->Begin(&cPasses,0);

	for(iPass = 0; iPass < cPasses; iPass++)
	{
		m_pBumpMapEffect->Get()->BeginPass(iPass);

		for (DWORD i=0; i < m_dwNumMaterials; i++)
		{
			// Set the material and texture for this subset
			D3DObj::Instance()->GetDeviceClass()->SetMaterial(&m_pMeshMaterials[i]);
			D3DObj::Instance()->GetDeviceClass()->SetTexture(0,m_pMeshTextures[i]);
			m_pBumpMapEffect->Get()->SetTexture("ModelTexture", m_pMeshTextures[i]);
			m_pBumpMapEffect->Get()->CommitChanges();
			m_pBumpMapEffect->Get()->SetTexture("NormalMap", m_bumpTextures[i]);
			m_pBumpMapEffect->Get()->CommitChanges();

			// Draw the mesh subset
			m_pMesh->DrawSubset( i );
		}

		m_pBumpMapEffect->Get()->EndPass();
	}
	m_pBumpMapEffect->Get()->End();
}

void Player::Update(float dt)
{
	GameObject::Update(dt);

}

void Player::Clean()
{
	for( DWORD i = 0; i < m_dwNumMaterials; i++ )
	{
		m_pMeshTextures[i]->Release();
	}
	m_pMesh->Release();
}

Effect* Player::GetEffect()
{
	return m_pBumpMapEffect;
}