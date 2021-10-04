#ifndef CONTEXTGL_H_GUARD
#define CONTEXTGL_H_GUARD

#include <windows.h>
#include <glad/glad.h>
#include <GL/wglext.h>
#include <iostream>

class ContextGl
{
private:
	HGLRC context;
	HWND hwnd;

public:
	HDC dc;
	ContextGl(HWND hWnd): hwnd(hWnd)
	{
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
			PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
			32,                   // Colordepth of the framebuffer.
			8, 0, 8, 0, 8, 0,
			8,
			0,
			0,
			0, 0, 0, 0,
			24,                   // Number of bits for the depthbuffer
			8,                    // Number of bits for the stencilbuffer
			0,                    // Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		dc = GetDC(hWnd);
		int format = ChoosePixelFormat(dc, &pfd);
		SetPixelFormat(dc, format, &pfd);

		auto tempContext = wglCreateContext (dc);
		if(!tempContext)
			MessageBox(hWnd, L"Couldn't create context", L"wglCreateContext", MB_OK);
		wglMakeCurrent(dc, tempContext);

		int attributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3, // Set the MAJOR version of OpenGL to 3
			WGL_CONTEXT_MINOR_VERSION_ARB, 2, // Set the MINOR version of OpenGL to 2
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB, // Set our OpenGL context to be forward compatible
			0
		};
		
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		if(wglCreateContextAttribsARB){
			context = wglCreateContextAttribsARB(dc, NULL, attributes);
			wglMakeCurrent(nullptr, nullptr); 
			wglDeleteContext(tempContext); 
			wglMakeCurrent(dc, context);
		
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
			wglSwapIntervalEXT(0); //VSYNC NOT LOCKED TO 60
		}
		else {
			tempContext = tempContext;
			std::cout<< "No OpenGL 3.2 Support, fallback Version 2.1"<<"\n";
		}
	}

	~ContextGl()
	{
		wglMakeCurrent (nullptr, nullptr);
		wglDeleteContext(context);
		ReleaseDC(hwnd, dc);
	}
};

#endif /* CONTEXTGL_H_GUARD */
