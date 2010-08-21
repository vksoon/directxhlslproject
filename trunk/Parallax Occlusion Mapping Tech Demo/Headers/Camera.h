#ifndef CAMERA_H
#define CAMERA_H

#include "InputManager.h"
#include "Singleton.h"

#include <string>

class Camera
{
	public:

		virtual const char * GetClassName() { return "Camera"; }

		~Camera();

		bool Load(LPCSTR type);
		void Update();
		void Move();
		void Rotate();

		D3DXMATRIX GetViewMatrix();
		D3DXMATRIX GetProjectionMatrix();
		D3DXMATRIX GetWorldMatrix();

		D3DXVECTOR3 GetEyePoint() const;

	private:

		Camera() {}
		friend class Singleton<Camera>;

		// eye point, look at point, up vector of world
		D3DXVECTOR3 m_vEyePoint, m_vLookAtPoint, m_vUp; 

		float m_fSpeed;

		D3DXMATRIX matProjection;  

		D3DXMATRIX m_matView; // view matrix
		D3DXMATRIX m_matProjection; // projection matrix
		D3DXMATRIX m_matWorld; // world matrix
		std::string m_type;
};
typedef Singleton<Camera> CameraObj;

#endif