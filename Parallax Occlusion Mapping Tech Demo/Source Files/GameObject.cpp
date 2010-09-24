#include "../Headers/GameObject.h"

GameObject::GameObject()
{
	m_pVBuffer = NULL;
	m_pTexture = NULL;

	m_vPos = 0;
	m_vVel = 0;
	m_vRot = 0;
	m_vOldpos = 0;
}

bool GameObject::Load(File * pFile)//std::string meshFile, std::string textureFile, std::string mapFile, std::string configFile)
{
	return true;
}

void GameObject::Draw()
{
}

void GameObject::SetPhysics()
{
}

Effect* GameObject::GetEffect()
{
	return m_pEffect;
}

void GameObject::Clean()
{
	m_pMesh->Release();
	m_pTexture->Release();
	m_pNormalTexture->Release();
}

void GameObject::Update(float dt)
{
	m_vOldpos = m_vPos;
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




