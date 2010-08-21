/*
	Direct3D Object Wrapper
	Singleton
	D3DObj::Instance

	Shaun Mitchell
*/
#ifndef D3DOBJECT_H
#define D3DOBJECT_H

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9tex.h>
#include "Singleton.h"

// include the Direct3D Library file
//#pragma comment (lib, "d3d9.lib")
//#pragma comment (lib, "d3dx9.lib")

#include <AntTweakBar.h>


class D3DObject
{
	public:
		~D3DObject();

		void Init(HWND hWnd);
		void Clean();

		LPDIRECT3D9 GetDevice() const; 
		LPDIRECT3DDEVICE9 GetDeviceClass() const;
		D3DPRESENT_PARAMETERS GetParameters() const;
		HWND GetWindowHandle() const;


	private:

		LPDIRECT3D9 d3d; // pointer to device 
		LPDIRECT3DDEVICE9 d3ddev; // pointer to device class
		D3DPRESENT_PARAMETERS d3dpp;
		D3DObject() {}
		friend class Singleton<D3DObject>;
};

typedef Singleton<D3DObject> D3DObj;

#endif