/*
--------------------
| GameState (Base)|
--------------------
|
----------------------
| MainState (Derived)|
----------------------

MainState Class
*/

#ifndef MAINSTATE_H
#define MAINSTATE_H

#include "GameState.h"
#include "GameObject.h"
#include "Player.h"
#include "MapObject.h"
#include "SkyBox.h"
#include "Camera.h"
#include "Factory.h"
#include <map>
#include <string>
#include "../Headers/ConfigFile.h"

class MainState : public GameState
{
public:

	bool Init();
	void Clean();

	void Pause();
	void Resume();

	void HandleInput(Game* game);
	void HandleCollisions(Game* game);
	void Update(Game* game, float dt);
	void Draw(Game* game);

	static MainState* Instance() {
		return &m_MainState;
	}

protected:
	MainState() { }

private:
	static MainState m_MainState;

	float waterMovement;

	// Values for TweakBar 
	float depth;
	int MinSamples;
	int MaxSamples;
	float TextureRepeat; 
	int LevelOfDetailThreshold;
};

#endif