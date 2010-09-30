/*
    --------------------
	| GameObject (Base)|
	--------------------

	Derive specific Game Objects

	Contains Get and Set for all Translation, Scaling and Rotation Matrices
	Contains Velocity
	Contains Mesh

	Shaun Mitchell
*/

#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <vector>
#include <assert.h>

#include "../Headers/Vec3.h"
#include "../Headers/Camera.h"
#include "../Headers/ParallaxOcclusionMappingEffect.h"
#include "../Headers/D3DObject.h"
#include "../Headers/Factory.h"

#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_TEX1)

struct CUSTOMVERTEX
{
    FLOAT x, y, z;   
	float u, v;
};

class GameObject
{
public:

	virtual ~GameObject() {}
	GameObject();

	virtual const char * GetClassName() { return "GameObject";}

	virtual bool Load(File * pFile);
	virtual void Draw();
	virtual void Update(float dt);
	virtual void Clean();

	virtual Effect* GetEffect();

	// Added Get and Set functions  // Position & Velocity
	float GetXPosition() const;
	float GetXVelocity() const;
	float GetYPosition() const;
	float GetYVelocity() const;
	float GetZPosition() const;
	float GetZVelocity() const;
	float GetOldX() const;
	float GetOldY() const;
	float GetOldZ() const;

	void SetXPosition(float xpos);
	void SetYPosition(float ypos);
	void SetZPosition(float zpos);
	void SetXVelocity(float xvel);
	void SetYVelocity(float yvel);
	void SetZVelocity(float zvel);

	// Added Get and Set functions // Scaling
	float GetXScale() const;
	float GetYScale() const;
	float GetZScale() const;

	void SetXScale(float xs);
	void SetYScale(float ys);
	void SetZScale(float zs);
	void SetOverallScale(float os);

	float GetXRotation() const;
	float GetYRotation() const;
	float GetZRotation() const;

	void SetXRotation(float xr);
	void SetYRotation(float yr);
	void SetZRotation(float zr);

	D3DXMATRIX GetWorldMatrix();
	D3DXMATRIX GetTranslate();

	LPD3DXMESH GetMesh() { return m_pMesh; }

	std::string GetID() {return m_id;}

protected:

	std::string m_id;

	Vec3f m_vPos;
	Vec3f m_vVel;
	Vec3f m_vRot;
	Vec3f m_vOldpos;
	Vec3f m_vScale;

	D3DXMATRIX m_matTranslate; // the translation matrix
	D3DXMATRIX m_matScaling; // the scale matrix
	D3DXMATRIX m_matRotationX; // the rotation matrix x
	D3DXMATRIX m_matRotationY;
	D3DXMATRIX m_matRotationZ;
	D3DXMATRIX m_matRot;
	D3DXMATRIX m_matWorld;

	Effect* m_pEffect;

	LPDIRECT3DVERTEXBUFFER9 m_pVBuffer;
	LPDIRECT3DTEXTURE9 m_pTexture;
	LPDIRECT3DTEXTURE9 m_pNormalTexture;

	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;

	LPD3DXMESH m_pMesh;    // define the mesh pointer

};
typedef std::vector<GameObject*>  GameObjects;

#endif