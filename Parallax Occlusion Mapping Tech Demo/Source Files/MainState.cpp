
#include <stdio.h>

#include "../Headers/Game.h"
#include "../Headers/MainState.h"
#include "../Headers/GameObject.h"
#include "../Headers/FontManager.h"
#include <assert.h>
#include <iostream>
#include <math.h>
#include <string>

MainState MainState::m_MainState;

bool MainState::Init()
{
	// Initialize defaults for Tweak Bar
	depth = 0.02f;
	MinSamples = 8;
	MaxSamples = 50;
	TextureRepeat = 5.0f;
	LevelOfDetailThreshold = 100;

	// Load Camera
	CameraObj::Instance()->Load("Static");

	// Create a font // 20 in size // OCR A Std
	FontManagerObj::Instance()->FMCreateFont("FirstFont", 17, "OCR A Std");

	// Get the view port to set accurate values in TweakBar
	D3DVIEWPORT9 mainViewPort;
	D3DObj::Instance()->GetDeviceClass()->GetViewport(&mainViewPort);

	UINT width = mainViewPort.Width;
	UINT height = mainViewPort.Height;

	// Init TweakBar
	TwInit(TW_DIRECT3D9, D3DObj::Instance()->GetDeviceClass());

	// Create a TweakBar
	TwBar * bar = TwNewBar("Shader Values");

	// Set window size in TweakBar
	TwWindowSize(width,height);

	TwDefine(" GLOBAL help='Change Colours and values of POM shader with tweak bar' "); // Message added to the help bar.

	TwAddVarRW(bar, "Depth", TW_TYPE_FLOAT, &depth, " label='Parallax Depth' min=0 max=0.1 step=0.01 ");
	TwAddVarRW(bar, "MinSamples", TW_TYPE_INT32, &MinSamples, " label='Min Samples' min=0 max=10 step=1 ");
	TwAddVarRW(bar, "MaxSamples", TW_TYPE_INT32, &MaxSamples, " label='Max Samples' min=50 max=100 step=1 ");
	TwAddVarRW(bar, "TextureRepeat", TW_TYPE_FLOAT, &TextureRepeat, " label='Texture Repeat' min=1 max=10 step=1 ");
	TwAddVarRW(bar, "LevelOfDetailThreshold", TW_TYPE_INT32, &LevelOfDetailThreshold, " label='Level of Detail' min=0 max=2000 step=10 ");

	waterMovement = 0;

	// Create a factory and register some GameObjects
	Factory<GameObject> GameObjectFactory;
	GameObjectFactory.Register("GameObject", new Creator<GameObject, GameObject>);
	GameObjectFactory.Register("Player", new Creator<Player, GameObject>);
	GameObjectFactory.Register("MapObject", new Creator<MapObject, GameObject>);
	GameObjectFactory.Register("SkyBox", new Creator<SkyBox, GameObject>);

	// Create a new file to open
	File config;

	// Open the file for this state
	config.Open("ConfigFiles/MainStateConfig.txt");

	// Load Game Objects from file
	while(!config.EndOfFile())
	{
		std::string typeName;
		config.GetString(&typeName);

		if (typeName.empty() || typeName[0] == '#')
		{
			return true;
		}

		GameObject* pMyNewGameObject = GameObjectFactory.Create(typeName);
		pMyNewGameObject->Load(&config);

		// Add Game Objects to m_gameObjects map
		std::string id = pMyNewGameObject->GetID();
		m_gameObjects[id] = pMyNewGameObject;
	}

	// Set some values in shader if needed // 
	float spec = 0;
	GetGameObject("Floor")->GetEffect()->Get()->SetValue("SpecularExponent", &spec, sizeof(spec));
	return true;
}

// Free textures and memory
void MainState::Clean()
{
	// Clean Game Objects
	for(GameObjects::iterator it = m_gameObjects.begin(); it != m_gameObjects.end(); ++it)
	{
		GameObject* p = it->second;
		p->Clean();
	}

	// Free Effects // vector is only for making sure effects get deleted.
	for(unsigned int i = 0; i < Effect::Effects.size(); i++)
	{
		if(!Effect::Effects[i]) continue;
		Effect::Effects[i]->Release();
	}

	// Release TweakBar
	TwTerminate();

	// Release fonts from Fonts Map
	FontManagerObj::Instance()->FMClean();
	InputManagerObj::Instance()->Clean();
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
	// Set camera type with mouse click 
	if(InputManagerObj::Instance()->MouseButtonDown(1))
	{
		CameraObj::Instance()->SetType("FirstPerson");
	}
	else
	{
		CameraObj::Instance()->SetType("Static");
	}
}

// Handle Collisions // None Needed
void MainState::HandleCollisions(Game* game)
{
}

// Update the state
void MainState::Update(Game* game, float dt) 
{
	// Update Game Objects
	for(GameObjects::iterator it = m_gameObjects.begin(); it != m_gameObjects.end(); ++it)
	{
		GameObject* p = it->second;
		p->Update(dt);
	}

	// Update Input Manager
	InputManagerObj::Instance()->Update();

	// Pass in the amount of texture movement for water movement
	waterMovement += dt / 100;
	GetGameObject("Water")->GetEffect()->Get()->SetValue("WaterMovement", &waterMovement, sizeof(waterMovement));

	
	// Values for TweakBar // Eugh! // TODO : Wrap up TweakBar
	GetGameObject("FrontWall")->GetEffect()->Get()->SetValue("HeightMapScale", &depth, sizeof(depth));
	GetGameObject("BackWall")->GetEffect()->Get()->SetValue("HeightMapScale", &depth, sizeof(depth));
	GetGameObject("LeftWall")->GetEffect()->Get()->SetValue("HeightMapScale", &depth, sizeof(depth));
	GetGameObject("RightWall")->GetEffect()->Get()->SetValue("HeightMapScale", &depth, sizeof(depth));
	GetGameObject("FrontWall")->GetEffect()->Get()->SetValue("MinSamples", &MinSamples, sizeof(MinSamples));
	GetGameObject("BackWall")->GetEffect()->Get()->SetValue("MinSamples", &MinSamples, sizeof(MinSamples));
	GetGameObject("LeftWall")->GetEffect()->Get()->SetValue("MinSamples", &MinSamples, sizeof(MinSamples));
	GetGameObject("RightWall")->GetEffect()->Get()->SetValue("MinSamples", &MinSamples, sizeof(MinSamples));
	GetGameObject("FrontWall")->GetEffect()->Get()->SetValue("MaxSamples", &MaxSamples, sizeof(MaxSamples));
	GetGameObject("BackWall")->GetEffect()->Get()->SetValue("MaxSamples", &MaxSamples, sizeof(MaxSamples));
	GetGameObject("LeftWall")->GetEffect()->Get()->SetValue("MaxSamples", &MaxSamples, sizeof(MaxSamples));
	GetGameObject("RightWall")->GetEffect()->Get()->SetValue("MaxSamples", &MaxSamples, sizeof(MaxSamples));
	GetGameObject("FrontWall")->GetEffect()->Get()->SetValue("BaseTextureRepeat", &TextureRepeat, sizeof(TextureRepeat));
	GetGameObject("BackWall")->GetEffect()->Get()->SetValue("BaseTextureRepeat", &TextureRepeat, sizeof(TextureRepeat));
	GetGameObject("LeftWall")->GetEffect()->Get()->SetValue("BaseTextureRepeat", &TextureRepeat, sizeof(TextureRepeat));
	GetGameObject("RightWall")->GetEffect()->Get()->SetValue("BaseTextureRepeat", &TextureRepeat, sizeof(TextureRepeat));
	GetGameObject("FrontWall")->GetEffect()->Get()->SetValue("LevelOfDetailThreshold", &LevelOfDetailThreshold, 
		sizeof(LevelOfDetailThreshold));
	GetGameObject("BackWall")->GetEffect()->Get()->SetValue("LevelOfDetailThreshold", &LevelOfDetailThreshold, 
		sizeof(LevelOfDetailThreshold));
	GetGameObject("LeftWall")->GetEffect()->Get()->SetValue("LevelOfDetailThreshold", &LevelOfDetailThreshold, 
		sizeof(LevelOfDetailThreshold));
	GetGameObject("RightWall")->GetEffect()->Get()->SetValue("LevelOfDetailThreshold", &LevelOfDetailThreshold, 
		sizeof(LevelOfDetailThreshold));
}

// Render the state
void MainState::Draw(Game* game) 
{
	D3DObj::Instance()->GetDeviceClass()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000, 1.0f, 1);

	D3DObj::Instance()->GetDeviceClass()->BeginScene();

	// Draw Game Objects
	for(GameObjects::iterator it = m_gameObjects.begin(); it != m_gameObjects.end(); ++it)
	{
		GameObject* p = it->second;
		p->Draw();
	}
	// Draw the tweak bar
	TwDraw();

	// Draw Fonts
	FontManagerObj::Instance()->FMDrawText("FirstFont", 250, 10, 700, 100, "Hold Right Mouse Button to Rotate Camera", 255, 255, 255);
	FontManagerObj::Instance()->FMDrawText("FirstFont", 250, 30, 700, 100, "           W A S D to Move Camera", 255, 255, 255);

	D3DObj::Instance()->GetDeviceClass()->EndScene();   

	D3DObj::Instance()->GetDeviceClass()->Present(NULL, NULL, NULL, NULL);
}
