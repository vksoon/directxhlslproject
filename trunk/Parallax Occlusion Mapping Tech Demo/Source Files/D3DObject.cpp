#include "../Headers/D3DObject.h"

D3DObject::~D3DObject() {}

void D3DObject::Init(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION); // create the interface
	
	// create a struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
    d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferHeight = 800;
	d3dpp.BackBufferWidth = 800;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // create a device class using this information and the info from the d3dpp stuct
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      hWnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                      &d3dpp,
                      &d3ddev);
	d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
}


void D3DObject::Clean()
{
	d3ddev->Release();    // close and release the 3D device
    d3d->Release();    // close and release Direct3D
}

LPDIRECT3D9 D3DObject::GetDevice() const
{
	return d3d;
}

LPDIRECT3DDEVICE9 D3DObject::GetDeviceClass() const
{
	return d3ddev;
}

D3DPRESENT_PARAMETERS D3DObject::GetParameters() const
{
	return d3dpp;
}

// Took a chance with this function, seems to work
HWND D3DObject::GetWindowHandle() const
{
	HWND hWnd;
	hWnd = FindWindow(NULL, L"hWnd");
	return hWnd;
}