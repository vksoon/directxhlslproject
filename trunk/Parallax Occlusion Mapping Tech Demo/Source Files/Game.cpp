#include "../Headers/Game.h"
#include "../Headers/InputManager.h"
#include <assert.h>
#include <stdio.h>
#include <sstream>
#include "../Headers/GameState.h"

void Game::Init()
{
	//D3DObj::Instance()->GetDeviceClass()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//D3DObj::Instance()->GetDeviceClass()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//D3DObj::Instance()->GetDeviceClass()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	//D3DObj::Instance()->GetDeviceClass()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	//D3DObj::Instance()->GetDeviceClass()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	//D3DObj::Instance()->GetDeviceClass()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );


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
	InputManagerObj::Instance()->Clean();
	TwTerminate();
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

void Game::Render()
{
	states.back()->Render(this);
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
