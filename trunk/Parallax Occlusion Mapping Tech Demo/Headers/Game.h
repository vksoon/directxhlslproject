/*
	Game.h

	Handles Game States
*/

#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <vector>

#include "D3DObject.h"

class GameState;

class Game
{
public:

	void Init();
	void Clean();

	void ChangeState(GameState* state);
	void PushState(GameState* state);
	void PopState();

	void HandleInput();
	void HandleCollisions();

	void Update(float dt);
	void Render();

private: 
	
	std::vector<GameState*> states;
};

#endif
