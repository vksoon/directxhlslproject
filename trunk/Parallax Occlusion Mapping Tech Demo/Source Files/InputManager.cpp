#include "../Headers/InputManager.h"

InputManager::~InputManager()
{
	
}

void InputManager::Clean()
{
	// release the input manager object
	m_pKeyboardDevice->Unacquire();
	m_pKeyboardDevice->Release();
	m_pKeyboardDevice = NULL;
	m_pInputManager->Release();
	m_pInputManager = NULL;
	m_pMouseDevice->Unacquire();
	m_pMouseDevice->Release();
	m_pMouseDevice = NULL;
}

bool InputManager::InitInputManager()
{
	m_fCursorX = (float)D3DObj::Instance()->GetParameters().BackBufferWidth / 2;
	m_fCursorY = (float)D3DObj::Instance()->GetParameters().BackBufferHeight / 2;

	if(FAILED(DirectInput8Create(GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputManager,
		NULL)))
	{
		return false;
	}
	return true;
}

bool InputManager::InitKeyboard()
{
	if(FAILED(m_pInputManager->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, NULL)))
	{
		return false;
	}

	if(FAILED(m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return false;
	}

	if(FAILED(m_pKeyboardDevice->SetCooperativeLevel(D3DObj::Instance()->GetWindowHandle(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		return false;
	}

	if(FAILED(m_pKeyboardDevice->Acquire()))
	{
		return false;
	}

	return true;
}

bool InputManager::InitMouse()
{
	DIDEVCAPS MouseCapabilities;

	if(FAILED(m_pInputManager->CreateDevice(GUID_SysMouse, &m_pMouseDevice, NULL)))
	{
		return false;
	}

	if(FAILED(m_pMouseDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return false;
	}

	if(FAILED(m_pMouseDevice->SetCooperativeLevel(D3DObj::Instance()->GetWindowHandle(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		return false;
	}

	if(FAILED(m_pMouseDevice->Acquire()))
	{
		return false;
	}

	SetCapture(D3DObj::Instance()->GetWindowHandle());


	MouseCapabilities.dwSize = sizeof(MouseCapabilities);
	m_pMouseDevice->GetCapabilities(&MouseCapabilities);

	if(!(MouseCapabilities.dwFlags & DIDC_ATTACHED))
	{
		return false;
	}

	m_dwAxes = MouseCapabilities.dwAxes;
	m_dwButtons = MouseCapabilities.dwButtons;

	SetCursor("Assets/stones.bmp",0,0,0xffffffff);
	SetAttributes(false, 1.0f);

	return true;
}

bool InputManager::Update()
{
	if(FAILED(m_pKeyboardDevice->GetDeviceState(sizeof(m_keyBuffer),(LPVOID)&m_keyBuffer)))
	{
		return false;
	}

	if(DIERR_INPUTLOST == m_pMouseDevice->GetDeviceState(sizeof(m_MouseState),(LPVOID)&m_MouseState))
	{
		m_pMouseDevice->Acquire();
	}

	m_fCursorX += m_MouseState.lX * m_fSensitivity;
	if(m_fCursorX < 0) m_fCursorX = 0;
	else if(m_fCursorX > 800) m_fCursorX = (float)800;

	if(m_bInverted) m_fCursorY -= m_MouseState.lY * m_fSensitivity;
	else m_fCursorY += m_MouseState.lY * m_fSensitivity;
	if(m_fCursorY < 0) m_fCursorY = 0;
	else if(m_fCursorY > 800) m_fCursorY = (float)800;

	return true;
}

void InputManager::DrawCursor()
{
	D3DCURSORVERTEX aCursorVertices[4];

	aCursorVertices[0].fX = m_fCursorX - m_iHotSpotX;
	aCursorVertices[0].fY = m_fCursorY - m_iHotSpotY;
	aCursorVertices[0].fZ = 0.0f;
	aCursorVertices[0].fRHW = 1.0f;
	aCursorVertices[0].dwColor = m_dwCursorColour;
	aCursorVertices[0].fU = 0.0f;
	aCursorVertices[0].fV = 0.0f;

	aCursorVertices[1].fX = m_fCursorX - m_iHotSpotX;
	aCursorVertices[1].fY = m_fCursorY + 32.0f - m_iHotSpotY;
	aCursorVertices[1].fZ = 0.0f;
	aCursorVertices[1].fRHW = 1.0f;
	aCursorVertices[1].dwColor = m_dwCursorColour;
	aCursorVertices[1].fU = 0.0f;
	aCursorVertices[1].fV = 1.0f;

	aCursorVertices[2].fX = m_fCursorX + 32.0f - m_iHotSpotX;
	aCursorVertices[2].fY = m_fCursorY - m_iHotSpotY;
	aCursorVertices[2].fZ = 0.0f;
	aCursorVertices[2].fRHW = 1.0f;
	aCursorVertices[2].dwColor = m_dwCursorColour;
	aCursorVertices[2].fU = 1.0f;
	aCursorVertices[2].fV = 0.0f;

	aCursorVertices[3].fX = m_fCursorX + 32.0f - m_iHotSpotX;
	aCursorVertices[3].fY = m_fCursorY + 32.0f - m_iHotSpotY;
	aCursorVertices[3].fZ = 0.0f;
	aCursorVertices[3].fRHW = 1.0f;
	aCursorVertices[3].dwColor = m_dwCursorColour;
	aCursorVertices[3].fU = 1.0f;
	aCursorVertices[3].fV = 1.0f;

	D3DObj::Instance()->GetDeviceClass()->SetFVF(D3DFVF_CURSORVERTEX);

	D3DObj::Instance()->GetDeviceClass()->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	D3DObj::Instance()->GetDeviceClass()->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	D3DObj::Instance()->GetDeviceClass()->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	D3DObj::Instance()->GetDeviceClass()->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);

	D3DObj::Instance()->GetDeviceClass()->SetTexture(0,m_pCursorTexture);
	D3DObj::Instance()->GetDeviceClass()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,&aCursorVertices[0],sizeof(D3DCURSORVERTEX));

	D3DObj::Instance()->GetDeviceClass()->SetFVF(D3DFVF_CURSORVERTEX);
}

void InputManager::SetCursor(const char* strFilename, int iCursorHotSpotX, int iCursorHotSpotY, DWORD dwColor)
{
	D3DXCreateTextureFromFileExA(D3DObj::Instance()->GetDeviceClass(),strFilename,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,0xff000000,NULL,NULL,&m_pCursorTexture);

m_iHotSpotX = iCursorHotSpotX;
m_iHotSpotY = iCursorHotSpotY;
m_dwCursorColour = dwColor;
}

void InputManager::SetAttributes(bool bInvert, float fNewSensitivity)
{
	m_bInverted = bInvert;
	m_fSensitivity = fNewSensitivity;
}

void InputManager::SetCursorPosition(float fNewX, float fNewY)
{
	m_fCursorX = fNewX;
	m_fCursorY = fNewY;
}

float InputManager::GetRelativeX()
{
	return (float)m_MouseState.lX * m_fSensitivity;
}

float InputManager::GetRelativeY(void)
{
	return (float)m_MouseState.lY * m_fSensitivity;
}

float InputManager::GetRelativeZ(void)
{
	return (float)m_MouseState.lZ * m_fSensitivity;
}

float InputManager::GetAbsoluteX(void)
{
	return m_fCursorX;
}

float InputManager::GetAbsoluteY(void)
{
	return m_fCursorY;
}



bool InputManager::KeyPressed(int Key)
{
	if(m_keyBuffer[Key] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputManager::MouseButtonDown(int Button)
{
	if(m_MouseState.rgbButtons[Button] & 0x80)
	{
		return true;
	}

	return false;
}
