//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: triangle.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders a triangle in wireframe mode.  Demonstrates vertex buffers, 
//       render states, and drawing commands.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "vector3.cpp"

//
// Globals
//
IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;


struct Vertex
{
	Vertex(){}

	Vertex(float x, float y, float z)
	{
		_x = x;	 _y = y;  _z = z;
	}

	float _x, _y, _z;

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ;

struct  Pixel
{
	unsigned short x, y;
	unsigned long color;
	Pixel(unsigned short nx, unsigned short ny) : x(nx), y(ny), color(0xffff0000){};
	Pixel(){};
};

LPDIRECT3DSURFACE9 p_surface;
DWORD* sysmemImage;
void DrawLine(Pixel start, Pixel end);
//
// Framework Functions
//
bool Setup()
{
	if (Device)
	{
		if (FAILED(Device->CreateOffscreenPlainSurface(Width, Height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &p_surface, 0)))
			return false;
		D3DLOCKED_RECT lockedRect;
		if (FAILED(p_surface->LockRect(&lockedRect, 0, D3DLOCK_DISCARD)))
		{
			return false;
		}
		DWORD* imageData = (DWORD*)lockedRect.pBits;
		for (UINT i = 0; i < Height; i++)
		{
			for (UINT j = 0; j < Width; j++)
			{
				imageData[i * Width + j] = 0xff646464;
			}
		}
		p_surface->UnlockRect();

		sysmemImage = new DWORD[Width * Height];
		ZeroMemory(sysmemImage, sizeof(DWORD)* Width * Height);

		DrawLine(Pixel(0, 0), Pixel(300, 300));
	}
	return p_surface;
}
void Cleanup()
{
	p_surface->Release();
}

void SetPixel(Pixel pixels[], int nCount)
{
	if (!sysmemImage)
	{
		return;
	}
	Pixel pixel;
	for (int i = 0; i < nCount; i++)
	{
		pixel = pixels[i];
		sysmemImage[pixel.x + pixel.y * Width] = pixel.color;
	}
}
void SetPixel(Pixel)
{

}

void DrawLine(Pixel start, Pixel end)
{
	int nWidth = abs(end.x - start.x);
	Pixel* pixels = new Pixel[nWidth];
	float fgradient = (float)(end.y - start.y) / (end.x - start.x);
	//Vector3 direction(1, , 0);
	for (int i = 0; i < nWidth; i++)
	{
		pixels[i] = Pixel(start.x + i, start.y + roundf(i * fgradient));
	}
	SetPixel(pixels, nWidth);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		D3DLOCKED_RECT lockedRect;
		if (FAILED(p_surface->LockRect(&lockedRect, 0, D3DLOCK_DISCARD)))
		{
			return false;
		}
		memcpy(lockedRect.pBits, sysmemImage, sizeof(DWORD)* Width* Height);
		p_surface->UnlockRect();

		LPDIRECT3DSURFACE9 backBuffer;
		Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_FORCE_DWORD, &backBuffer);
		Device->StretchRect(p_surface, 0, backBuffer, 0, D3DTEXF_LINEAR);
		//Device->rect
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}


//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}