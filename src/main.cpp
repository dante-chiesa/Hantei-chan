#include "main.h"
#include "context_gl.h"
#include "main_frame.h"
#include "test.h"
#include "ini.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <imgui.h>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_win32.h>
#include <windows.h>
#include <shellapi.h>

#include <glad/glad.h>

#include <res/resource.h>

#ifndef HA6GUIVERSION
#define HA6GUIVERSION " custom"
#endif

char iniLocation[512] {};

ImVec2 clientRect;

HWND mainWindowHandle;
ContextGl *context = nullptr;
static bool dragLeft = false, dragRight = false;
static POINT mousePos;
bool init = false;

void LoadJapaneseFonts(ImGuiIO& io)
{
	char winFolder[512]{};
	ImFontConfig config;
/* 	config.PixelSnapH = 1;
	config.OversampleH = 1;
	config.OversampleV = 1; */
	int appendAt = GetWindowsDirectoryA(winFolder, 512);
	strcpy(winFolder+appendAt, "\\Fonts\\meiryo.ttc");
	if(!io.Fonts->AddFontFromFileTTF(winFolder, gSettings.fontSize, &config, io.Fonts->GetGlyphRangesJapanese()))
	{
		auto res = FindResource((HMODULE)GetWindowLongPtr(mainWindowHandle, GWLP_HINSTANCE), MAKEINTRESOURCE(NOTO_SANS_JP_F), RT_RCDATA);
		void *notoFont = LockResource(LoadResource(nullptr, res));
		config.FontDataOwnedByAtlas = false;
		
		io.Fonts->AddFontFromMemoryTTF(notoFont, SizeofResource(nullptr, res), gSettings.fontSize, &config, io.Fonts->GetGlyphRangesJapanese());
	}
}


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	bool useIni = true;
	int argC;
	PWSTR* argV = CommandLineToArgvW(pCmdLine, &argC);
	for(int i=0; i<argC; i++)
	{
		char * arg = (char*)(argV[i]);
		wcstombs(arg, argV[i], wcslen(argV[i])+1);
		if(!strcmp(arg, "--test"))
		{
			std::ofstream coutFile;
			coutFile.open("cout.txt");
			auto cout_buf = std::cout.rdbuf(coutFile.rdbuf());
			TestPat();
			LocalFree(argV);
			return 0;
		}
		else if(!strcmp(arg, "-i"))
		{
			useIni = false;
		}
		else if(!strcmp(arg, "-r") && i+2<argC)
		{
			i+=1;
			int res[2];
			for(int j = 0; j < 2; j++)
			{
				char * arg = (char*)(argV[i+j]);
				wcstombs(arg, argV[i+j], wcslen(argV[i+j])+1);
				res[j] = atoi(arg);
			}
			gSettings.winSizeX = res[0];
			gSettings.winSizeY = res[1];
		}
	}

	//If it fails... Well, that works too.
	if(useIni)
	{
		int appendAt = GetCurrentDirectoryA(512, iniLocation);
		strcpy(iniLocation+appendAt, "\\hanteichan.ini");
	}
	
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0L, 0L,
		hInstance,
		NULL, NULL, NULL, NULL,
		L"Main window", NULL 
	};

	
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, L"判定ちゃん v" HA6GUIVERSION, WS_OVERLAPPEDWINDOW,
		gSettings.posX, gSettings.posY, gSettings.winSizeX, gSettings.winSizeY, NULL, NULL, wc.hInstance, nullptr);
	mainWindowHandle = hwnd;

	init = true;
	if(useIni)
		ShowWindow(hwnd, gSettings.maximized ? SW_SHOWMAXIMIZED : SW_NORMAL);
	else
		ShowWindow(hwnd, nCmdShow);
		
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		MainFrame* mf = (MainFrame*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(mf)
			mf->Draw();
	}

	DestroyWindow(hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	LocalFree(argV);
	return 0;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	MainFrame* mf = (MainFrame*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch (msg)
	{
	case WM_CREATE:
		{
			context = new ContextGl(hWnd);
			if(!gladLoadGL())
			{
				MessageBox(0, L"glad fail", L"gladLoadGL()", 0);
				PostQuitMessage(1);
				return 1;
			}
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.IniFilename = iniLocation;
			InitIni();

			MainFrame* mf = new MainFrame(context);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)mf);
			
			MoveWindow(hWnd, gSettings.posX, gSettings.posY, gSettings.winSizeX, gSettings.winSizeY, false);
			

			LoadJapaneseFonts(io);
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			ImGui_ImplWin32_Init(hWnd);
			ImGui_ImplOpenGL3_Init("#version 120");
			
			return 0;
		}
	case WM_MOVE:
		{
			RECT rect;
			GetWindowRect(hWnd, &rect);
			gSettings.posX = rect.left;
			gSettings.posY = rect.top;
		}
		break;
	case WM_SIZE:
		if(init)
		{
			if (wParam == SIZE_MAXIMIZED)
				gSettings.maximized = true;
			else if (wParam == SIZE_RESTORED)
				gSettings.maximized = false;
			if (wParam != SIZE_MINIMIZED)
			{
				RECT rect;
				GetWindowRect(hWnd, &rect);
				gSettings.posX = rect.left;
				gSettings.posY = rect.top;
				gSettings.winSizeX = rect.right-rect.left;
				gSettings.winSizeY = rect.bottom-rect.top;


				GetClientRect(hWnd, &rect);
				clientRect = ImVec2((float)rect.right, (float)rect.bottom);
				mf->UpdateBackProj(clientRect.x, clientRect.y);
			}
		}
		return 0;
	case WM_KEYDOWN:
		if(!ImGui::GetIO().WantCaptureKeyboard)
		{
			if(mf->HandleKeys(wParam))
				return 0;
		}
		break;
	case WM_RBUTTONDOWN:
		if(!ImGui::GetIO().WantCaptureMouse)
		{
			dragRight = true;
			GetCursorPos(&mousePos);
			ScreenToClient(hWnd, &mousePos);
			SetCapture(hWnd);
			mf->RightClick(mousePos.x, mousePos.y);
			
			return 0;
		}
		break;
	case WM_RBUTTONUP:
		if(dragRight)
		{
			if(!dragLeft)
				ReleaseCapture();
			dragRight = false;
			return 0;
		}
		break;
	case WM_LBUTTONDOWN:
		if(!ImGui::GetIO().WantCaptureMouse)
		{
			dragLeft = true;
			GetCursorPos(&mousePos);
			ScreenToClient(hWnd, &mousePos);
			SetCapture(hWnd);
			
			return 0;
		}
		break;
	case WM_LBUTTONUP:
		if(dragLeft)
		{
			if(!dragRight)
				ReleaseCapture();
			dragLeft = false;
			return 0;
		}
		break;
	case WM_MOUSEMOVE:
		if(dragLeft || dragRight)
		{
			POINT newMousePos;
			newMousePos.x = (short) LOWORD(lParam);
			newMousePos.y = (short) HIWORD(lParam);

			mf->HandleMouseDrag(newMousePos.x-mousePos.x, newMousePos.y-mousePos.y, dragRight, dragLeft);
			mousePos = newMousePos;
		}
		break;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		delete mf;
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		delete context;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
