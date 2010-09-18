#include "../Headers/Game.h"
#include "../Headers/InputManager.h"
#include <assert.h>
#include <stdio.h>
#include <sstream>
#include "../Headers/GameState.h"

void Game::Init()
{
	InputManagerObj::Instance()->InitInputManager();
	InputManagerObj::Instance()->InitKeyboard();
	InputManagerObj::Instance()->InitMouse();
}

void Game::Clean()
{
	while(!states.empty())
	{
		states.back()->Clean();
		states.pop_back();
	}
}

void Game::ChangeState(GameState* state)
{
	if(!states.empty())
	{
		states.back()->Clean();
		states.pop_back();
	}

	states.push_back(state);
	states.back()->Init();
}

void Game::PushState(GameState* state)
{
	if(!states.empty())
	{
		states.back()->Pause();
	}

	states.push_back(state);
	states.back()->Init();
}

void Game::PopState()
{
	if(!states.empty())
	{
		states.back()->Clean();
		states.pop_back();
	}

	if(!states.empty())
	{
		states.back()->Resume();
	}
}

void Game::Draw()
{
	states.back()->Draw(this);
}

void Game::HandleInput()
{
	states.back()->HandleInput(this);
}

void Game::Update(float dt)
{
	states.back()->Update(this, dt);
}

void Game::HandleCollisions ()
{
	states.back()->HandleCollisions(this);
}

