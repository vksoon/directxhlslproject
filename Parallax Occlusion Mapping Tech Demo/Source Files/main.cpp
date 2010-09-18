#include "../Headers/D3DObject.h"
#include "../Headers/Game.h"

#include "../Headers/MainState.h"
#include "../Headers/Camera.h"

#include <stdlib.h>

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "WindowClass";

	RegisterClassEx(&wc);

    hWnd = CreateWindowEx(NULL,
                          "WindowClass",
                          "Parallax Occlusion Mapping",
                          WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
                          300, 10,
                          800, 800,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);

	D3DObj::Instance()->Init(hWnd);

	GameInst::Instance()->Init();
	GameInst::Instance()->ChangeState(MainState::Instance());

	// enter the main loop:

    MSG msg;

	__int64 countsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);

	float secsPerCount = 1.0f / (float)countsPerSec;

	__int64 prevTimestamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimestamp);

    while(TRUE)
    {


        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(msg.message == WM_QUIT)
            break;

		__int64 currentTimestamp =  0;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTimestamp);
		float dt = (currentTimestamp  - prevTimestamp)* secsPerCount;

		GameInst::Instance()->Update(dt);
		GameInst::Instance()->Draw();
		GameInst::Instance()->HandleInput();

		CameraObj::Instance()->Update();
		
		prevTimestamp = currentTimestamp;
    }

    // clean up DirectX and COM
	D3DObj::Instance()->Clean();
	GameInst::Instance()->Clean();

    return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Send event message to AntTweakBar
    if( TwEventWin(hWnd, message, wParam, lParam) )
        return 0;   // Event has been handled by AntTweakBar

    switch(message)
    {
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}