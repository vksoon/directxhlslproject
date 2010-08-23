#include "../Headers/Camera.h"

Camera::~Camera() {}

bool Camera::Load(LPCSTR type)
{
	m_type = type;
	m_vEyePoint = D3DXVECTOR3(0.0f,0.0f,-150.0f);
	m_vLookAtPoint = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vUp = D3DXVECTOR3(0.0f,1.0f,0.0f);
	m_fSpeed = 1.0f;
	D3DXMatrixPerspectiveFovLH(&matProjection,
                               D3DXToRadian(45),    // the horizontal field of view
                               (float)800/(float)800, // aspect ratio
                               1.0f,    // the near view-plane
                               1000.0f);    // the far view-plane
    D3DObj::Instance()->GetDeviceClass()->SetTransform(D3DTS_PROJECTION, &matProjection);     // set the projection

	return true;
}

void Camera::Move()
{
	D3DXVECTOR3 vDirection;

	D3DXVec3Normalize(&vDirection, &(m_vLookAtPoint - m_vEyePoint)); // camera direction vector

	if(InputManagerObj::Instance()->KeyPressed(DIK_W))
	{
		m_vEyePoint += vDirection * m_fSpeed;
		m_vLookAtPoint += vDirection * m_fSpeed;
	}

	if(InputManagerObj::Instance()->KeyPressed(DIK_S))
	{
		m_vEyePoint -= vDirection * m_fSpeed;
		m_vLookAtPoint -= vDirection * m_fSpeed;
	}

	if(InputManagerObj::Instance()->KeyPressed(DIK_A))
	{
		D3DXVec3Cross(&vDirection,&vDirection,&m_vUp);
		D3DXVec3Normalize(&vDirection,&vDirection);
		m_vEyePoint += vDirection * m_fSpeed;
		m_vLookAtPoint += vDirection * m_fSpeed;
	}

	if(InputManagerObj::Instance()->KeyPressed(DIK_D))
	{
		D3DXVec3Cross(&vDirection,&vDirection,&m_vUp);
		D3DXVec3Normalize(&vDirection,&vDirection);
		m_vEyePoint -= vDirection * m_fSpeed;
		m_vLookAtPoint -= vDirection * m_fSpeed;
	}
}

void Camera::Rotate()
{
	D3DXVECTOR3 vDirection, vRotAxis;
	D3DXMATRIX matRotAxis, matRotY;

	D3DXVec3Normalize(&vDirection, &(m_vLookAtPoint - m_vEyePoint));
	D3DXVec3Cross(&vRotAxis, &vDirection, &m_vUp); //strafe vector
	D3DXVec3Normalize(&vRotAxis, &vRotAxis);

	// create rotation matrices
	D3DXMatrixRotationAxis(&matRotAxis, &vRotAxis,InputManagerObj::Instance()->GetRelativeY() / -360);

	D3DXMatrixRotationY(&matRotY, InputManagerObj::Instance()->GetRelativeX() / 360);

	//rotate direction
	D3DXVec3TransformCoord(&vDirection,&vDirection,&(matRotAxis * matRotY));
	//rotate up vector
	D3DXVec3TransformCoord(&m_vUp,&m_vUp,&(matRotAxis * matRotY));
	//translate up vector
	m_vLookAtPoint = vDirection + m_vEyePoint;

}

void Camera::Update()
{
	if(m_type == "FirstPerson")
	{
	Rotate();
	Move();
	}
	

	D3DXMatrixLookAtLH(&m_matView, &m_vEyePoint, &m_vLookAtPoint, &m_vUp);

	D3DObj::Instance()->GetDeviceClass()->SetTransform(D3DTS_VIEW, &m_matView);
}

D3DXMATRIX Camera::GetViewMatrix()
{
	return m_matView;
}

D3DXMATRIX Camera::GetProjectionMatrix()
{
	return matProjection;
}

D3DXMATRIX Camera::GetWorldMatrix()
{
	return m_matWorld;
}

D3DXVECTOR3 Camera::GetEyePoint() const
{
	return m_vEyePoint;
}