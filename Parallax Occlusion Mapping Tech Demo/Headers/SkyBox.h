/*
    --------------------
	| GameObject (Base)|
	--------------------
			|
	----------------------
	| SkyBox (Derived)|
	----------------------

	SkyBox Class

	Shaun Mitchell
*/
#ifndef SKYBOX_H
#define SKYBOX_H

#include "../Headers/GameObject.h"

#include <vector>

class SkyBox : public GameObject
{
public:

	virtual const char * GetClassName() { return "SkyBox";}

	bool Load(File * pFile);
	void Draw();
	void Update(float dt);
	void Clean();

private:

	D3DMATERIAL9  *m_pMeshMaterials;
	LPDIRECT3DTEXTURE9 *m_pMeshTextures;
	DWORD              m_dwNumMaterials;

};

#endif