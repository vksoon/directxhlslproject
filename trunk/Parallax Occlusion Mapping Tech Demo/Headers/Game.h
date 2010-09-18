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
#include <map>
#include <string>

#include "D3DObject.h"
#include "GameObject.h"

class GameState;

class Game
{
public:

	~Game() {}

	void Init();
	void Clean();

	void ChangeState(GameState* state);
	void PushState(GameState* state);
	void PopState();

	void HandleInput();
	void HandleCollisions();

	void Update(float dt);
	void Draw();

	void UpdateGameObjects(float dt);
	void DrawGameObjects();
	void CleanGameObjects();


private: 

	Game() {}
	friend class Singleton<Game>;
	
	std::vector<GameState*> states;
};
typedef Singleton<Game> GameInst;

#endif
