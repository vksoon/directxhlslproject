/*
	DirectInput
	Input Manager 
	Shaun Mitchell
*/
#ifndef _INPUT_MANAGER_H_ 
#define _INPUT_MANAGER_H_

#include <windows.h>

#define DIRECTINPUT_VERSION (0x0800)

//constants for mouse buttons (NEW)
#define DIMOUSE_LEFTBUTTON   0
#define DIMOUSE_RIGHTBUTTON  1
#define DIMOUSE_MIDDLEBUTTON 2
#define DIMOUSE_4BUTTON      3
#define DIMOUSE_5BUTTON      4
#define DIMOUSE_6BUTTON      5
#define DIMOUSE_7BUTTON      6
#define DIMOUSE_8BUTTON      7

//vertex format for cursor (NEW)
#define D3DFVF_CURSORVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

struct D3DCURSORVERTEX
{
float fX,
      fY,
      fZ,
      fRHW;
DWORD dwColor;
float fU,
      fV;
};

#include <dinput.h>
#include "D3DObject.h"

class InputManager
{
public:

	//InputManager();
	~InputManager();

	// Initialise Direct Input
	bool InitInputManager();
	// Initialise Keyboard
	bool InitKeyboard();
	// Initialise Mouse
	bool InitMouse();
	// Update the Keyboards Status
	bool Update();
	// Check if Key is Pressed
	bool KeyPressed(int Key);

	// Mouse Specific
	void DrawCursor();
	void SetCursor(const char*, int,int, DWORD);
	void SetAttributes(bool, float);
	void SetCursorPosition(float, float);
	void Clean();

	// Get relative movement
	float GetRelativeX();
	float GetRelativeY();
	// Wheel movement
	float GetRelativeZ();
	// Cursor Position
	float GetAbsoluteX();
	float GetAbsoluteY();
	// Check mouse button
	bool MouseButtonDown(int);

private:

	LPDIRECTINPUT8 m_pInputManager; // The Input Object
	LPDIRECTINPUTDEVICE8 m_pKeyboardDevice; // The Keyboard
	LPDIRECTINPUTDEVICE8 m_pMouseDevice; // the Mouse
	
	char m_keyBuffer[256]; // Key Buffer

	// Mouse Specific
	LPDIRECT3DTEXTURE9 m_pCursorTexture; // cursor texture

	// Mouse state structure
	DIMOUSESTATE2 m_MouseState;
	bool m_bInverted;
	float m_fSensitivity, m_fCursorX, m_fCursorY;
	int m_iHotSpotX, m_iHotSpotY;
	DWORD m_dwAxes, m_dwButtons, m_dwCursorColour, m_dwScreenWidth, m_dwScreenHeight;

	InputManager() {}
	friend class Singleton<InputManager>;
};

typedef Singleton<InputManager> InputManagerObj;

#endif