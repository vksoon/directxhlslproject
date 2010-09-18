#include "../Headers/SkyBox.h"

bool SkyBox::Load(File * pFile)
{
	std::string id;
	std::string meshFile;
	float xrot,yrot,zrot,xpos,ypos,zpos,xscale,yscale,zscale, uniformscale;

	pFile->GetString(&id);
	pFile->GetString(&meshFile);
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

	// Create the material buffer for textures from the .x file
	LPD3DXBUFFER pD3DXMtrlBuffer;

	// Load the mesh from the specified file
	if( FAILED(D3DXLoadMeshFromX( meshFile.c_str(), D3DXMESH_MANAGED,
		D3DObj::Instance()->GetDeviceClass(), NULL,
		&pD3DXMtrlBuffer, NULL, &m_dwNumMaterials,
		&m_pMesh )))
	{
		MessageBox(NULL, "Could not find Skybox Mesh file", "Mesh Loading", MB_OK);
		return false;
	}

	D3DXComputeNormals(m_pMesh, NULL);

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

void SkyBox::Draw()
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

	D3DObj::Instance()->GetDeviceClass()->SetTransform( D3DTS_WORLD, &mWorld);

	D3DXMATRIXA16 mView;
	D3DXMATRIXA16 mProjection;

	mView = CameraObj::Instance()->GetViewMatrix();
	mProjection = CameraObj::Instance()->GetProjectionMatrix();

	D3DXMatrixMultiply(&mWV, &mWorld, &mView);
	D3DXMatrixMultiply(&mWVP, &mWV, &mProjection);

	D3DXVECTOR3 EyePoint = CameraObj::Instance()->GetEyePoint();

	for (DWORD i=0; i < m_dwNumMaterials; i++)
	{
		D3DObj::Instance()->GetDeviceClass()->SetMaterial(&m_pMeshMaterials[i]);
		D3DObj::Instance()->GetDeviceClass()->SetTexture(0,m_pMeshTextures[i]);

		// Draw the mesh subset
		m_pMesh->DrawSubset( i );
	}
}

void SkyBox::Update(float dt)
{
	GameObject::Update(dt);

}

void SkyBox::Clean()
{
	for( DWORD i = 0; i < m_dwNumMaterials; i++ )
	{
		m_pMeshTextures[i]->Release();
	}
	m_pMesh->Release();
}
