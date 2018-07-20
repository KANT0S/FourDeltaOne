// ==========================================================
// IW5M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Add screenshot commands.
//
// Initial author: NTAuthority 
// Started: 2012-20-11
// ==========================================================

#include "StdInc.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <comdef.h>
#include <image/jpeglib.h>

typedef void (__cdecl * Scr_AddString_t)(const char* string);
char* (__cdecl * DXGetErrorString)(HRESULT hr) = (char* (__cdecl *)(HRESULT hr))0x4B8ED0;
tagMONITORINFO mi;
tagPOINT Point;
IDirect3DDevice9 *device;
HRESULT hr = S_OK;    
IDirect3DSurface9* surface;
IDirect3DSwapChain9* swapchain;
HWND Window;
HMONITOR Monitor;

bool fexists(const char *filename)
{
	FILE* file = fopen(filename, "r");
	if(file)
	{
		fclose(file);
		return true;
	}
	return false;
}
char* Screenshot_GetFileName(char* ext)
{
	const char* fname;
	for(int i = 0; i < 10000; i++)
	{
		fname = va("screenshots/%d.%s", i, ext);
		if(!fexists(fname))
		{
			return (char*)fname;
		}
	}
	return (char*)fname;
}

bool TakeScreenshot(void* DATA, int window_width, int window_height)
{
	Point.x = 0;
	Point.y = 0;
	int width = 0;
	int height = 0;
	device = *(IDirect3DDevice9**)0x05F96FA8;
	swapchain = *(IDirect3DSwapChain9**)0x5F99C7C;
	Window = *(HWND*)0x5A86AF0;
	if (*(DWORD*)0x5F9C418) // fullscreen
	{
		width = window_width;
		height = window_height;
	}
	else
	{
		Monitor = MonitorFromWindow(Window, 2);
		mi.cbSize = sizeof(mi);
		if (!GetMonitorInfoA(Monitor, &mi) )
		{
		  Com_Printf(8, "ERROR: cannot take screenshot: couldn't get screen dimensions\n");
		  return false;
		}
		ClientToScreen(Window, &Point);

		if ( *(signed __int64 *)&Point < *(signed __int64 *)&mi.rcMonitor.left || window_width + Point.x > mi.rcMonitor.right || Point.y + window_height > mi.rcMonitor.bottom )
		{
			Com_Printf(8, "ERROR: cannot take screenshot: game window is partially off-screen\n");
			return false;
		}

		Point.x -= mi.rcMonitor.left;
		width = mi.rcMonitor.right - mi.rcMonitor.left;
		height = mi.rcMonitor.bottom - mi.rcMonitor.top;
		Point.y -= mi.rcMonitor.top;
	}
	if (FAILED(hr = device->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &surface, NULL)))
	{
		Com_Printf(8, "ERROR: cannot take screenshot: couldn't create the off-screen surface: %s (0x%08x)\n", DXGetErrorString(hr), hr);
		if(surface) surface->Release();
		return false;
	}
	if (FAILED(hr = swapchain->GetFrontBufferData(surface)))
	{
		surface->Release();
		surface = NULL;
		Com_Printf(8, "ERROR: cannot take screenshot: couldn't create the off-screen surface: %s (0x%08x)\n", DXGetErrorString(hr), hr);
		return false;
	}
	D3DLOCKED_RECT lock;
	RECT rect;
	rect.left = Point.x;
	rect.top = window_width + Point.x;
	rect.right = Point.y;
	rect.bottom = window_height + Point.y;
	if (FAILED(hr = surface->LockRect(&lock, &rect, D3DLOCK_READONLY)))
	{
		surface->Release();
		Com_Printf(8, "ERROR: cannot take screenshot: LockRect failed: %s (0x%08x)\n", DXGetErrorString(hr), hr);
		return false;
	}
	
	void* DATA_TEMP = DATA;
	int temp_height = window_height;
	int something = lock.Pitch - 4 * window_width;
	do
	{
		int temp_width = window_width;
		do
		{
			*(BYTE*)DATA_TEMP = *(BYTE*)lock.pBits;
			*(BYTE*)(&DATA_TEMP + 1) = *(BYTE*)(&lock.pBits + 1);
			*(BYTE*)(&DATA_TEMP + 2) = *(BYTE*)(&lock.pBits + 2);
			*(void**)DATA_TEMP = &DATA_TEMP + 3;
			*(void**)(lock.pBits) = &lock.pBits + 4;
			--temp_width;
		} while(temp_width);
		*(void**)(lock.pBits) = &lock.pBits + something;
	} while (temp_height-- != 1);

	surface->UnlockRect();
	surface->Release();

	return true;
}

void Cmd_Screenshot()
{
	int window_width = *(DWORD*)0x5F9C400;
	int window_height = *(DWORD*)0x5F9C404;
	void* DATA = malloc(3 * window_width * window_height);
	if(!DATA)
	{
		Com_Printf(8, "ERROR: cannot take screenshot: cannot allocate %d bytes\n", 3 * window_width * window_height);
		return;
	}

	if(!TakeScreenshot(DATA, window_width, window_height))
	{
		free(DATA);
		return;
	}

	char* fname = Screenshot_GetFileName("jpg");
	FILE* outfile = fopen(fname, "wb");

	jpeg_compress_struct cinfo;
	jpeg_error_mgr       jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width      = window_width;
	cinfo.image_height     = window_height;
	cinfo.input_components = 3;
	cinfo.in_color_space   = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality (&cinfo, 75, true);
	jpeg_start_compress(&cinfo, true);

	JSAMPROW row_pointer[1];
	for(int i =0; i < window_width * window_height * 3; i++)
	{
		row_pointer[0] = *(JSAMPROW*)(&DATA + cinfo.next_scanline * cinfo.image_width * cinfo.input_components);
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);

	free(DATA);
}

void Screenshots()
{
	return;
	static cmd_function_s screenshot;
	static cmd_function_s screenshotjpeg;
	static cmd_function_s screenshottga;
	Cmd_AddCommand("screenshot", Cmd_Screenshot, &screenshot);
	Cmd_AddCommand("screenshotJPEG", Cmd_Screenshot, &screenshotjpeg);
	Cmd_AddCommand("screenshotTGA", Cmd_Screenshot, &screenshottga);
}