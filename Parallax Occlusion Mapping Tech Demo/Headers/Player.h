#ifndef PLAYER_H
#define PLAYER_H

#include "../Headers/GameObject.h"

#include <vector>

class Player : public GameObject
{
public:

	bool Load(LPCWSTR meshFile, LPCWSTR textureFile, LPCWSTR mapFile);
	void Render();
	void Update(float dt);
	void Clean();

private:

	D3DMATERIAL9  *m_pMeshMaterials;
	LPDIRECT3DTEXTURE9 *m_pMeshTextures;
	DWORD              m_dwNumMaterials;
	LPDIRECT3DTEXTURE9 textures[9];

	LPDIRECT3DTEXTURE9 armorBump;
	LPDIRECT3DTEXTURE9 bodyBump;
	LPDIRECT3DTEXTURE9 headBump;
	LPDIRECT3DTEXTURE9 headgearBump;
	LPDIRECT3DTEXTURE9 helmetBump;
	LPDIRECT3DTEXTURE9 packBump;
	LPDIRECT3DTEXTURE9 weaponsBump;

};

#endif