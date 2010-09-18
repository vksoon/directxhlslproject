#include "../Headers/MapObject.h"

bool MapObject::Load(File * pFile)
{
	std::string id;
	std::string meshFile, textureFile, mapFile, shaderConfigFile;
	float xrot,yrot,zrot,xpos,ypos,zpos,xscale,yscale,zscale, uniformscale;

	pFile->GetString(&id);
	pFile->GetString(&meshFile);
	pFile->GetString(&textureFile);
	pFile->GetString(&mapFile);
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

	//load texture from file (NEW)
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		textureFile.c_str(),
		&m_pTexture);

	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		mapFile.c_str(),
		&m_pNormalTexture);

	D3DXLoadMeshFromX(meshFile.c_str(), D3DXMESH_MANAGED, D3DObj::Instance()->GetDeviceClass(), NULL, 
		NULL, NULL, NULL, &m_pMesh);

	// Create an Effect Factory and Register some Creator Objects
	Factory<Effect> EffectFactory;
	EffectFactory.Register("ParallaxOcclusionMappingEffect", new Creator<ParallaxOcclusionMappingEffect, Effect>);

	// Create the Lighting Effect file
	m_pParallaxEffect = EffectFactory.Create("ParallaxOcclusionMappingEffect");
	assert(!_stricmp(m_pParallaxEffect->GetClassName(), "ParallaxOcclusionMappingEffect")); // Check the class name exists

	m_pParallaxEffect->Load("Shaders/ParallaxOcclusionMapping.fx");
	m_pParallaxEffect->LoadValuesFromConfig(shaderConfigFile);

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

	return true;
}

void MapObject::Draw()
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

	m_pParallaxEffect->GetEffect()->SetValue("EyeLocation", &EyePoint, sizeof(EyePoint));

	m_pParallaxEffect->GetEffect()->SetValue("World", &mWorld, sizeof(mWorld));
	m_pParallaxEffect->GetEffect()->SetValue("WorldViewProjection", &mWVP, sizeof(mWVP));

	m_pParallaxEffect->GetEffect()->SetValue("View", &mView, sizeof(mView));

	m_pParallaxEffect->GetEffect()->SetTexture("BaseTexture", m_pTexture);
	m_pParallaxEffect->GetEffect()->SetTexture("HeightMapTexture", m_pNormalTexture);

	// Get Inverse Transpose of World Matrix
	D3DXMATRIX m;
	D3DXMatrixInverse(&m, NULL, &mWorld);
	D3DXMATRIX j;
	D3DXMatrixTranspose(&j, &m );
	D3DXMATRIX inv = j;

	//m_pEffect->GetEffect()->SetMatrix("WorldInverseTranspose", &inv);

	m_pParallaxEffect->GetEffect()->SetTechnique("ParallaxOcclusionMapping");
	UINT iPass, cPasses;

	m_pParallaxEffect->GetEffect()->Begin(&cPasses,0);

	for(iPass = 0; iPass < cPasses; iPass++)
	{
		m_pParallaxEffect->GetEffect()->BeginPass(iPass);

		// Draw the mesh subset
		m_pMesh->DrawSubset(0);

		m_pParallaxEffect->GetEffect()->EndPass();
	}
	m_pParallaxEffect->GetEffect()->End();

}

void MapObject::Update(float dt)
{
	GameObject::Update(dt);
}

void MapObject::Clean()
{
	GameObject::Clean();
}

Effect* MapObject::GetEffect()
{
	return m_pParallaxEffect;
}