
#include <stdio.h>

#include "../Headers/Game.h"
#include "../Headers/MainState.h"
#include "../Headers/GameObject.h"
#include <assert.h>
#include <iostream>

MainState MainState::m_MainState;

float m_angleY = 0;
float m_angleZ = 0;

D3DXVECTOR4 AmbientColour(0.5,0.5,0.5,0.5);
D3DXVECTOR4 AmbientColour2(1,0,0,1);
D3DXVECTOR4 DiffuseColour(1,1,1,1);
D3DXVECTOR4 SpecularColour(1,1,1,1);
D3DCOLOR backgroundColour = 0x0000ff;

bool Active;

float HeightMapScale = 0.0f;
float SpecularExponent = 10000.0f;
float ShadowSoftening = 0.0f;


void MainState::Init()
{
	// Camera
	CameraObj::Instance()->Load("FirstPerson");

	waterMovement = 0;

	UINT width = D3DObj::Instance()->GetParameters().BackBufferWidth;
	UINT height = D3DObj::Instance()->GetParameters().BackBufferHeight;

	TwInit(TW_DIRECT3D9, D3DObj::Instance()->GetDeviceClass());

	TwBar *pShaderBar = TwNewBar("ShaderValues");

	TwWindowSize(width, height);

	TwDefine(" GLOBAL help='Change Colours and values of POM shader with tweak bar' "); // Message added to the help bar.
	TwDefine("ShaderValues color='255 0 0' text=light "); // Change TweakBar color and use dark text

	TwAddVarRW(pShaderBar, "AmbientColour", TW_TYPE_COLOR3F, &AmbientColour, " label='Ambient Material Colour' colormode=rgb");
	TwAddVarRW(pShaderBar, "DiffuseColour", TW_TYPE_COLOR3F, &DiffuseColour, " label='Diffuse Material Colour' colormode=rgb");
	TwAddVarRW(pShaderBar, "SpecularColour", TW_TYPE_COLOR3F, &SpecularColour, " label='Specular Material Colour' colormode=rgb");
	TwAddVarRW(pShaderBar, "HeightMapScale", TW_TYPE_FLOAT, &HeightMapScale, " label='Height Map Scale' min=0 max=0.1 step=0.01");
	TwAddVarRW(pShaderBar, "SpecularExponent", TW_TYPE_FLOAT, &SpecularExponent, " label='Specular Exponent' min=0 max=10000 step=1.0");
	TwAddVarRW(pShaderBar, "ShadowSoftening", TW_TYPE_FLOAT, &ShadowSoftening, " label='Shadow Softening' min=0 max=10.0 step=0.1");

	TwAddVarRW(pShaderBar, "backgroundColour", TW_TYPE_COLOR32, &backgroundColour, " label='Background Colour' colormode=rgb");

	pBottomFloor = new GameObject();
	pBottomFloor->Load(L"Assets/plane.x", L"Assets/floor.jpg", L"Assets/floor_NRM.png"); 

	pFloor = new GameObject();
	pFloor->Load(L"Assets/plane.x", L"Assets/water.png", L"Assets/water_NRM.png"); 

	pSky = new GameObject();
	pSky->Load(L"Assets/plane.x", L"Assets/sky.jpg", L"Assets/sky.jpg"); 

	pRightWall = new GameObject();
	pRightWall->Load(L"Assets/plane.x", L"Assets/stones.bmp", L"Assets/stones_NM_height.tga");

	pLeftWall = new GameObject();
	pLeftWall->Load(L"Assets/plane.x", L"Assets/stones.bmp", L"Assets/stones_NM_height.tga");

	pBackWall = new GameObject();
	pBackWall->Load(L"Assets/plane.x", L"Assets/stones.bmp", L"Assets/stones_NM_height.tga");

	pFrontWall = new GameObject();
	pFrontWall->Load(L"Assets/plane.x", L"Assets/stones.bmp", L"Assets/stones_NM_height.tga");


	pPlayer = new Player();
	pPlayer->Load(L"DwarfWithEffectInstance.x", NULL, NULL);

	pFloor->SetOverallScale(1.0f);
	pBottomFloor->SetOverallScale(1.0f);
	pPlayer->SetOverallScale(20.0f);
	pPlayer->SetXPosition(0.0f);
	pPlayer->SetYPosition(-30.0f);
	pPlayer->SetZPosition(0.0f);
	pFloor->SetXPosition(0.0f);
	pFloor->SetYPosition(10.0f);
	pFloor->SetZPosition(0.0f);
	pBottomFloor->SetXPosition(0.0f);
	pBottomFloor->SetYPosition(0.0f);
	pBottomFloor->SetZPosition(0.0f);
	D3DXVECTOR4 specularColour(0,0,0,0);
	pBottomFloor->GetEffect()->GetEffect()->SetValue("MaterialSpecularColour", &specularColour, sizeof(specularColour));
	

	float trfloor = 5.0;
	float blend = true;
	pFloor->GetEffect()->GetEffect()->SetValue("BaseTextureRepeat", &trfloor, sizeof(trfloor));
	pFloor->GetEffect()->GetEffect()->SetValue("MaterialSpecularColour", &specularColour, sizeof(specularColour));
	pFloor->GetEffect()->GetEffect()->SetValue("transparency", &blend, sizeof(blend));

	pRightWall->SetZRotation(D3DX_PI / 2);
	pRightWall->SetXPosition(25.0f);
	pRightWall->SetYPosition(0.0f);
	pRightWall->SetZPosition(0.0f);
	pRightWall->SetOverallScale(1.0f);

	pBackWall->SetXRotation(-D3DX_PI / 2);
	pBackWall->SetZRotation(D3DX_PI / 2);
	pBackWall->SetXPosition(-30.0f);
	pBackWall->SetYPosition(0.0f);
	pBackWall->SetZPosition(0.0f);
	pBackWall->SetOverallScale(1.0f);

	pFrontWall->SetXRotation(D3DX_PI / 2);
	pFrontWall->SetZRotation(D3DX_PI / 2);
	pFrontWall->SetXPosition(30.0f);
	pFrontWall->SetYPosition(0.0f);
	pFrontWall->SetZPosition(-120.0f);
	pFrontWall->SetOverallScale(1.0f);

	pLeftWall->SetZRotation(-D3DX_PI / 2);
	pLeftWall->SetXPosition(-30.0f);
	pLeftWall->SetYPosition(0.0f);
	pLeftWall->SetZPosition(0.0f);
	pLeftWall->SetOverallScale(1.0f);

	pSky->SetZRotation(D3DX_PI);
	pSky->SetXPosition(0.0f);
	pSky->SetYPosition(30.0f);
	pSky->SetZPosition(0.0f);
	pSky->SetOverallScale(1.0f);
	float tr = 1.0;
	pSky->GetEffect()->GetEffect()->SetValue("BaseTextureRepeat", &tr, sizeof(tr));

	GameObject::GameObjects.push_back(pBottomFloor);
	GameObject::GameObjects.push_back(pSky);
	GameObject::GameObjects.push_back(pRightWall);
	GameObject::GameObjects.push_back(pLeftWall);
	GameObject::GameObjects.push_back(pBackWall);
	GameObject::GameObjects.push_back(pFrontWall);
	GameObject::GameObjects.push_back(pPlayer);
	GameObject::GameObjects.push_back(pFloor);
}

// Free textures and memory
void MainState::Clean()
{

	for(unsigned int i = 0;i < GameObject::GameObjects.size();i++) {
		if(!GameObject::GameObjects[i]) continue;

		GameObject::GameObjects[i]->Clean();
	}

	// Free Effects // vector is only for making sure effects get deleted.
	for(unsigned int i = 0; i < Effect::Effects.size(); i++)
	{
		if(!Effect::Effects[i]) continue;
		Effect::Effects[i]->Release();
	}
}

// Pause this state
void MainState::Pause()
{
}

// Resume this state
void MainState::Resume()
{
}

// Handle Input using InputManager
void MainState::HandleInput(Game* game)
{
	if(InputManagerObj::Instance()->KeyPressed(DIK_RIGHT))
	{
		m_angleY += 0.1f;
	}

	if(InputManagerObj::Instance()->KeyPressed(DIK_LEFT))
	{
		m_angleY -= 0.1f;
	}
}

// Handle Collisions // None Needed
void MainState::HandleCollisions(Game* game)
{
}

// Update the state
void MainState::Update(Game* game, float dt) 
{
	if(Active)
	{
		pFloor->GetEffect()->GetEffect()->SetValue("MaterialAmbientColour", &AmbientColour2, sizeof(AmbientColour2));
	}
	pPlayer->SetYRotation(m_angleY);
	pFloor->GetEffect()->GetEffect()->SetValue("MaterialAmbientColour", &AmbientColour, sizeof(AmbientColour));
	pFloor->GetEffect()->GetEffect()->SetValue("MaterialDiffuseColour", &DiffuseColour, sizeof(DiffuseColour));
	pFloor->GetEffect()->GetEffect()->SetValue("MaterialSpecularColour", &SpecularColour, sizeof(SpecularColour));
	pFloor->GetEffect()->GetEffect()->SetValue("HeightMapScale", &HeightMapScale, sizeof(HeightMapScale));
	pFloor->GetEffect()->GetEffect()->SetValue("SpecularExponent", &SpecularExponent, sizeof(SpecularExponent));
	pFloor->GetEffect()->GetEffect()->SetValue("ShadowSoftening", &ShadowSoftening, sizeof(ShadowSoftening));

	for(unsigned int i = 0;i < GameObject::GameObjects.size();i++) {
		if(!GameObject::GameObjects[i]) continue;

		GameObject::GameObjects[i]->Update(dt);
	}
	InputManagerObj::Instance()->Update();

	waterMovement += dt / 100;
	pFloor->GetEffect()->GetEffect()->SetValue("dt", &waterMovement, sizeof(waterMovement));
}

// Render the state
void MainState::Render(Game* game) 
{

	D3DObj::Instance()->GetDeviceClass()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, backgroundColour, 1.0f, 1);

	D3DObj::Instance()->GetDeviceClass()->BeginScene();

	for(unsigned int i = 0; i < GameObject::GameObjects.size(); i++) {
		if(!GameObject::GameObjects[i]) continue;

		GameObject::GameObjects[i]->Render();
	}

	TwDraw();

	D3DObj::Instance()->GetDeviceClass()->EndScene();   

	D3DObj::Instance()->GetDeviceClass()->Present(NULL, NULL, NULL, NULL);
}
