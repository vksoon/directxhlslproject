/*
    --------------------
	| GameObject (Base)|
	--------------------
			|
	----------------------
	| Player (Derived)|
	----------------------

	Player Class
	
	Use to create players using bump mapping effect
	
	Contains BumpMapEffect * 

	Contains an array of bump map textures loaded in with the textures from .x file
	not dynamic but suitable for this demo. A custom model type with support for 2 material
	textures would need to be created.

	Shaun Mitchell
*/
#ifndef PLAYER_H
#define PLAYER_H

#include "../Headers/GameObject.h"
#include "../Headers/BumpMapEffect.h"

#include <vector>

class Player : public GameObject
{
public:

	virtual const char * GetClassName() { return "Player";}

	bool Load(File * pFile);
	void Draw();
	void Update(float dt);
	void Clean();

	Effect* GetEffect();


private:

	D3DMATERIAL9  *m_pMeshMaterials;
	LPDIRECT3DTEXTURE9 *m_pMeshTextures;
	DWORD              m_dwNumMaterials;
	LPDIRECT3DTEXTURE9 m_bumpTextures[9];

	Effect* m_pBumpMapEffect;

	LPDIRECT3DTEXTURE9 armorBump;
	LPDIRECT3DTEXTURE9 bodyBump;
	LPDIRECT3DTEXTURE9 headBump;
	LPDIRECT3DTEXTURE9 headgearBump;
	LPDIRECT3DTEXTURE9 helmetBump;
	LPDIRECT3DTEXTURE9 packBump;
	LPDIRECT3DTEXTURE9 weaponsBump;

};

#endif