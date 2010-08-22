/*
	MainState
*/

#ifndef MAINSTATE_H
#define MAINSTATE_H

#include "GameState.h"
#include "GameObject.h"
#include "Player.h"
#include "Camera.h"

class MainState : public GameState
{
public:
	void Init();
	void Clean();
	
	void Pause();
	void Resume();
	
	void HandleInput(Game* game);
	void HandleCollisions(Game* game);
	void Update(Game* game, float dt);
	void Render(Game* game);
	
	static MainState* Instance() {
		return &m_MainState;
	}
	
protected:
		MainState() { }
	
private:
	static MainState m_MainState;

	GameObject * pFloor;
	GameObject * pPlayer;
	GameObject * pRightWall;
	GameObject * pLeftWall;
	GameObject * pBackWall;
	GameObject * pSky;
};

#endif