#include <windows.h>
#include <stdio.h>
#include <xinput.h>
#include "block.h"

#define ID_MENU_NEW_GAME 1
#define ID_MENU_EXIT_GAME 2

static bool RunningGame = true;
static GameKeys Keys = {};
static GameGraphics Graphics = {};
static GameSession Session = {};

LRESULT CALLBACK
WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LRESULT Result = 0;
	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		WPARAM VKCode = wParam;
		if (VKCode == 'S')
		{
			Keys.Down = true;
		}
		if (VKCode == 'A')
		{
			Keys.Left = true;
		}
		if (VKCode == 'D')
		{
			Keys.Right = true;
		}
		if (VKCode == 'J')
		{
			Keys.Rotate = true;
		}
		if (VKCode == VK_SPACE)
		{
			Keys.Drop = true;
		}
	} break;
	case WM_KEYUP:
	{
		WPARAM VKCode = wParam;
		if (VKCode == 'S')
		{
			Keys.Down = false;
		}
		else if (VKCode == 'A')
		{
			Keys.Left = false;
		}
		else if (VKCode == 'D')
		{
			Keys.Right = false;
		}
		else if (VKCode == 'J')
		{
			Keys.Rotate = false;
		}
		else if (VKCode == VK_SPACE)
		{
			Keys.Drop = false;
		}
	} break;
	case WM_CLOSE:
	{
		RunningGame = false;
		// GameShutdown();
	} break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_MENU_EXIT_GAME:
		{
			RunningGame = false;
		} break;
		case ID_MENU_NEW_GAME:
		{
			// TODO: More Error Checking
			Keys = {};
			GameStart(&Graphics, &Session);
		}
		} break;
	} break;
	default:
		Result = DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	return Result;
}


int WINAPI
wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
	LARGE_INTEGER PerfCountFrequencyResult;
	QueryPerformanceFrequency(&PerfCountFrequencyResult);
	int64_t PerfCountFrequency = PerfCountFrequencyResult.QuadPart;

	// TODO: Fix control flow to avoid potential errors.
	LPCWSTR lpszClassName = L"BlockGameWindowClass";

	WNDCLASSW BlockGameWindowClass = {};
	BlockGameWindowClass.lpfnWndProc = WindowProc;
	BlockGameWindowClass.hInstance = hInstance;
	BlockGameWindowClass.lpszClassName = lpszClassName;

	RegisterClassW(&BlockGameWindowClass); // TODO: Error Checking

	DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE;

	// Calculate the proper width and height needed to create a window with a default client area of 640x480
	// TODO: Implement resizing ~ current client area sized is fixed.
	RECT WindowArea = { 0, 0, GameWindowWidth, GameWindowHeight };
	AdjustWindowRectEx(&WindowArea, dwStyle, true, 0);

	LONG WindowWidth = WindowArea.right - WindowArea.left;
	LONG WindowHeight = WindowArea.bottom - WindowArea.top;

	// Create the main render window.
	HWND hWnd = CreateWindowExW(
		0,
		lpszClassName,
		L"Block",
		dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth, WindowHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	// Create Render Buffer
	void* BitmapBuffer = VirtualAlloc(0, sizeof(uint32_t) * GameWindowWidth * GameWindowHeight, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (BitmapBuffer == NULL)
	{
		MessageBoxW(hWnd, L"Unable to allocate enough memory for the backbuffer.", L"Memory Error", MB_OK | MB_ICONWARNING);
		return 0; //We should probably cleanup.
	}

	BITMAPINFO BitmapInfo = {};
	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = GameWindowWidth;
	BitmapInfo.bmiHeader.biHeight = -GameWindowHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;


	Graphics.Buffer = BitmapBuffer;
	Graphics.Width = GameWindowWidth;
	Graphics.Height = GameWindowHeight;


	bool DidGameInitalize = GameInitialize(&Graphics, &Session);
	if (!DidGameInitalize)
	{
		MessageBoxW(hWnd, L"Unable to initalize the game session.", L"Memory Error", MB_OK | MB_ICONWARNING);
		return 0; //We should probably cleanup.
	}

	// Delete the ability to resize the window.
	HMENU hMenu = GetSystemMenu(hWnd, FALSE);
	DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
	DeleteMenu(hMenu, SC_SIZE, MF_BYCOMMAND);

	// Add menu for starting a new game, exiting the game, looking at saved scores.
	HMENU hMenubar = CreateMenu();
	HMENU hFileMenu = CreatePopupMenu();
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hFileMenu, L"&Menu");
	AppendMenuW(hFileMenu, MF_STRING, ID_MENU_NEW_GAME, L"New Game");
	AppendMenuW(hFileMenu, MF_STRING, ID_MENU_EXIT_GAME, L"Exit");

	SetMenu(hWnd, hMenubar);

	// Game Update Rate
	float GameUpdateRateHz = 60;
	float TargetFrameRateSeconds = 1.0f / (float)GameUpdateRateHz;

	LARGE_INTEGER LastCounter;
	QueryPerformanceCounter(&LastCounter);
	// End Game Update Rate

	// Main event loop
	MSG msg;
	while (RunningGame)
	{
		while (PeekMessageW(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		if (Session.State == Finished)
		{
			continue;
		}

		XINPUT_STATE ControllerState;
		if (XInputGetState(0, &ControllerState) == ERROR_SUCCESS)
		{
			XINPUT_GAMEPAD* Pad = &ControllerState.Gamepad;

			if ((Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN))
			{
				Keys.Down = true;
			}
			else
			{
				Keys.Down = false;
			}


			if ((Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT))
			{
				Keys.Left = true;
			}
			else
			{
				Keys.Left = false;
			}

			if ((Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) {
				Keys.Right = true;
			}
			else
			{
				Keys.Right = false;
			}

			if ((Pad->wButtons & XINPUT_GAMEPAD_B))
			{
				Keys.Rotate = true;
			}
			else
			{
				Keys.Rotate = false;
			}

			if ((Pad->wButtons & XINPUT_GAMEPAD_A))
			{
				Keys.Drop = true;
			}
			else
			{
				Keys.Drop = false;
			}
		}

		// Game Render Here
		GameUpdate(&Graphics, &Session, Keys);

		LARGE_INTEGER EndCounter;
		QueryPerformanceCounter(&EndCounter);

		int64_t CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
		float SPF = (float)CounterElapsed / (float)PerfCountFrequency;
		while (SPF < TargetFrameRateSeconds)
		{
			LARGE_INTEGER EndCounter;
			QueryPerformanceCounter(&EndCounter);
			CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
			SPF = (float)CounterElapsed / (float)PerfCountFrequency;
			// TODO: Don't liquify the CPU.
		}

		QueryPerformanceCounter(&EndCounter);
		LastCounter = EndCounter;

		HDC hdc = GetDC(hWnd);
		StretchDIBits(hdc,
			0, 0, GameWindowWidth, GameWindowHeight, // TODO: ~ window resizing
			0, 0, GameWindowWidth, GameWindowHeight,
			BitmapBuffer,
			&BitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY
		);
		ReleaseDC(hWnd, hdc);

		if (Session.State == Finished)
		{
			wchar_t Buffer[512];
			wsprintfW(Buffer, L"Score: %d\nLines: %d\nLevel: %d\n", Session.Score, Session.LinesCleared, Session.Level);
			MessageBoxW(hWnd, Buffer, L"Block", MB_OK | MB_ICONWARNING);
		}
	}

	if (DidGameInitalize)
	{
		GameShutdown(&Session);
	}

	// Cleanup Render Buffer
	if (BitmapBuffer != NULL)
		VirtualFree(BitmapBuffer, 0, MEM_RELEASE);

	// Unregister the Window Class
	UnregisterClassW(lpszClassName, hInstance);

	return 0;
}