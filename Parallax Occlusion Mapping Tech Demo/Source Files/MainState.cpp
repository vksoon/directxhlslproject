
#include <stdio.h>

#include "../Headers/Game.h"
#include "../Headers/MainState.h"
#include "../Headers/GameObject.h"
#include <assert.h>
#include <iostream>

MainState MainState::m_MainState;

float m_angleY = 0;
float m_angleZ = 0;

D3DXVECTOR4 AmbientColour(1,1,1,1);
D3DXVECTOR4 AmbientColour2(1,0,0,1);
D3DXVECTOR4 DiffuseColour(0,0,0,0);
D3DXVECTOR4 SpecularColour(1,1,1,1);
D3DCOLOR backgroundColour = 0x0000ff;

bool Active;

float HeightMapScale = 0.0f;
float SpecularExponent = 1000.0f;
float ShadowSoftening = 0.0f;


void MainState::Init()
{
	// Camera
	CameraObj::Instance()->Load("Static");

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
	TwAddVarRW(pShaderBar, "SpecularExponent", TW_TYPE_FLOAT, &SpecularExponent, " label='Specular Exponent' min=0 max=1000 step=1.0");
	TwAddVarRW(pShaderBar, "ShadowSoftening", TW_TYPE_FLOAT, &ShadowSoftening, " label='Shadow Softening' min=0 max=1.0 step=0.1");

	TwAddVarRW(pShaderBar, "backgroundColour", TW_TYPE_COLOR32, &backgroundColour, " label='Background Colour' colormode=rgb");

	pCube = new GameObject();
	pCube->Load();

	pCube->SetOverallScale(1.0f);

	GameObject::GameObjects.push_back(pCube);
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
		m_angleY += 0.01f;
	}

	if(InputManagerObj::Instance()->KeyPressed(DIK_LEFT))
	{
		m_angleY -= 0.01f;
	}

	if(InputManagerObj::Instance()->KeyPressed(DIK_UP))
	{
		m_angleZ -= 0.01f;
	}

	if(InputManagerObj::Instance()->KeyPressed(DIK_DOWN))
	{
		m_angleZ += 0.01f;

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
	   pCube->GetEffect()->GetEffect()->SetValue("MaterialAmbientColour", &AmbientColour2, sizeof(AmbientColour2));
	}
	pCube->SetZRotation(m_angleZ);
	pCube->SetYRotation(m_angleY);
	pCube->GetEffect()->GetEffect()->SetValue("MaterialAmbientColour", &AmbientColour, sizeof(AmbientColour));
	pCube->GetEffect()->GetEffect()->SetValue("MaterialDiffuseColour", &DiffuseColour, sizeof(DiffuseColour));
	pCube->GetEffect()->GetEffect()->SetValue("MaterialSpecularColour", &SpecularColour, sizeof(SpecularColour));
	pCube->GetEffect()->GetEffect()->SetValue("HeightMapScale", &HeightMapScale, sizeof(HeightMapScale));
	pCube->GetEffect()->GetEffect()->SetValue("SpecularExponent", &SpecularExponent, sizeof(SpecularExponent));
	pCube->GetEffect()->GetEffect()->SetValue("ShadowSoftening", &ShadowSoftening, sizeof(ShadowSoftening));

	for(unsigned int i = 0;i < GameObject::GameObjects.size();i++) {
		if(!GameObject::GameObjects[i]) continue;

		GameObject::GameObjects[i]->Update(dt);
	}
	InputManagerObj::Instance()->Update();
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
