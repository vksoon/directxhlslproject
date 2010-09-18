/*
	--------------------
	| GameState (Base) |
	--------------------

	Derive Game Specific Game States

	Shaun Mitchell
*/

#ifndef _GAME_STATE_H_
#define _GAME_STATE_H_

#include "Game.h"

class GameState
{
public:

	std::map<std::string, GameObject*> GetGameObjects() { return m_gameObjects; }
	GameObject* GetGameObject(std::string id) { return m_gameObjects[id]; }

	virtual bool Init() = 0;
	virtual void Clean() = 0;

	virtual void Pause() = 0;
	virtual void Resume() = 0;

	virtual void HandleInput(Game* game)= 0;
	virtual void HandleCollisions(Game* game) = 0;
	virtual void Update(Game* game, float dt) = 0;
	virtual void Draw(Game* game) = 0;

	void ChangeState(Game* game, GameState* state)
	{
		game->ChangeState(state);
	}

protected:
	GameState() {}

	typedef std::map<std::string, GameObject*> GameObjects;
	GameObjects m_gameObjects;
};

#endif