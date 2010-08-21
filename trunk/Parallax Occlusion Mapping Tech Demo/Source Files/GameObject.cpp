#include "../Headers/GameObject.h"

std::vector<GameObject*> GameObject::GameObjects;

GameObject::GameObject()
{
	m_pVBuffer = NULL;
	m_pIBuffer = NULL;
	m_pTexture = NULL;

	m_vPos = 0;
	m_vVel = 0;
	m_vRot = 0;
	m_vOldpos = 0;
}

bool GameObject::Load()
{
	//load texture from file (NEW)
	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		L"Assets/rocks.jpg",
		&m_pTexture);

	D3DXCreateTextureFromFile(D3DObj::Instance()->GetDeviceClass(),
		L"Assets/rocks_NM_height.tga",
		&m_pNormalTexture);

	D3DXLoadMeshFromX(L"Assets/Disc.x", D3DXMESH_SYSTEMMEM, D3DObj::Instance()->GetDeviceClass(), NULL, 
		NULL, NULL, NULL, &meshBox);

	m_pEffect = new Effect();

	m_pEffect->Load(L"Shaders/ParallaxOcclusionMapping.fx");

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
	rgdwAdjacency = new DWORD[ meshBox->GetNumFaces() * 3 ];

	meshBox->GenerateAdjacency( 1e-6f, rgdwAdjacency );

	D3DXCleanMesh(D3DXCLEAN_BOWTIES, meshBox, rgdwAdjacency, &tempMesh, rgdwAdjacency, NULL);

	meshBox = tempMesh;

	tempMesh->Release();

	LPD3DXMESH clonedMesh, newMesh;

	meshBox->CloneMesh(D3DXMESH_VB_MANAGED, decl, D3DObj::Instance()->GetDeviceClass(), &clonedMesh);

	meshBox->Release();

	D3DXComputeNormals(clonedMesh, rgdwAdjacency);

	D3DXComputeTangentFrameEx(clonedMesh, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
		D3DDECLUSAGE_BINORMAL, 0,
		D3DDECLUSAGE_NORMAL, 0, 0, rgdwAdjacency, -1.01f,
		-0.01f, -1.01f, &newMesh, NULL );

	clonedMesh->Release();

	meshBox = newMesh;
	newMesh = 0;

	return true;
}

void GameObject::Render()
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

	m_pEffect->GetEffect()->SetValue("EyeLocation", &EyePoint, sizeof(EyePoint));

	m_pEffect->GetEffect()->SetValue("World", &mWorld, sizeof(mWorld));
	m_pEffect->GetEffect()->SetValue("WorldViewProjection", &mWVP, sizeof(mWVP));

	m_pEffect->GetEffect()->SetValue("View", &mView, sizeof(mView));

	m_pEffect->GetEffect()->SetTexture("BaseTexture", m_pTexture);
	m_pEffect->GetEffect()->SetTexture("HeightMapTexture", m_pNormalTexture);

	// Get Inverse Transpose of World Matrix
	D3DXMATRIX m;
	D3DXMatrixInverse(&m, NULL, &mWorld);
	D3DXMATRIX j;
	D3DXMatrixTranspose(&j, &m );
	D3DXMATRIX inv = j;

	//m_pEffect->GetEffect()->SetMatrix("WorldInverseTranspose", &inv);

	m_pEffect->GetEffect()->SetTechnique("ParallaxOcclusionMapping");
	UINT iPass, cPasses;

	m_pEffect->GetEffect()->Begin(&cPasses,0);

	for(iPass = 0; iPass < cPasses; iPass++)
	{
		m_pEffect->GetEffect()->BeginPass(iPass);

		// Draw the mesh subset
		meshBox->DrawSubset(0);

		m_pEffect->GetEffect()->EndPass();
	}
	m_pEffect->GetEffect()->End();

}

Effect* GameObject::GetEffect()
{
	return m_pEffect;
}

void GameObject::Clean()
{
	meshBox->Release();
	m_pTexture->Release();
	m_pNormalTexture->Release();
	//m_pVBuffer->Release();
}

void GameObject::Update(float dt)
{
	m_vOldpos = m_vPos;
	// Update velocity and position
	Vec3f u = m_vVel;
	m_vPos += (m_vVel + u) * (dt * 0.5f);
}

float GameObject::GetXPosition() const
{
	return m_vPos.X();
}

float GameObject::GetXVelocity() const
{
	return m_vVel.X();
}

float GameObject::GetYPosition() const
{
	return m_vPos.Y();
}

float GameObject::GetYVelocity() const
{
	return m_vVel.Y();
}

float GameObject::GetZPosition() const
{
	return m_vPos.Z();
}

float GameObject::GetZVelocity() const
{
	return m_vVel.Z();
}

float GameObject::GetOldX() const
{
	return m_vOldpos.X();
}

float GameObject::GetOldY() const
{
	return m_vOldpos.Y();
}

float GameObject::GetOldZ() const
{
	return m_vOldpos.Z();
}

void GameObject::SetXPosition(float pos)
{
	m_vPos.SetX(pos);
}

void GameObject::SetXVelocity(float xvel)
{
	m_vVel.SetX(xvel);
}

void GameObject::SetYPosition(float pos)
{
	m_vPos.SetY(pos);
}

void GameObject::SetYVelocity(float yvel)
{
	m_vVel.SetY(yvel);
}

void GameObject::SetZPosition(float pos)
{
	m_vPos.SetZ(pos);
}

void GameObject::SetZVelocity(float zvel)
{
	m_vVel.SetZ(zvel);
}

D3DXMATRIX GameObject::GetWorldMatrix()
{
	return m_matWorld;
}

D3DXMATRIX GameObject::GetTranslate()
{
	return m_matTranslate;
}

// ------------- Scale -------------------------------------------------

float GameObject::GetXScale() const
{
	return m_vScale.X();
}

float GameObject::GetYScale() const
{
	return m_vScale.Y();
}

float GameObject::GetZScale() const
{
	return m_vScale.Z();
}

void GameObject::SetXScale(float xs)
{
	m_vScale.SetX(xs);
}

void GameObject::SetYScale(float ys)
{
	m_vScale.SetY(ys);
}

void GameObject::SetZScale(float zs)
{
	m_vScale.SetZ(zs);
}

void GameObject::SetOverallScale(float os)
{
	m_vScale.SetX(os);
	m_vScale.SetY(os);
	m_vScale.SetZ(os);
}

float GameObject::GetXRotation() const
{
	return m_vRot.X();
}

float GameObject::GetYRotation() const
{
	return m_vRot.Y();
}

float GameObject::GetZRotation() const
{
	return m_vRot.Z();
}

void GameObject::SetXRotation(float xr)
{
	m_vRot.SetX(xr);
}

void GameObject::SetYRotation(float yr)
{
	m_vRot.SetY(yr);
}

void GameObject::SetZRotation(float zr)
{
	m_vRot.SetZ(zr);
}




