#include <windows.h>
#include <xinput.h>
#include "block.h"

static bool RunningGame = true;
static GameKey Key = None;
static GameKey ControllerKey = None;

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
			Key = Down;
		}
		else if (VKCode == 'A')
		{
			Key = Left;
		}
		else if (VKCode == 'D')
		{
			Key = Right;
		}
		else if (VKCode == 'J')
		{
			Key = Rotate;
		}
		else if (VKCode == VK_SPACE)
		{
			Key = Drop;
		}
	} break;
	case WM_CLOSE:
	{
		RunningGame = false;
		// GameShutdown();
	} break;
	default:
		Result = DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	return Result;
}


int WINAPI
wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
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

	// Delete the ability to resize the window.
	HMENU hMenu = GetSystemMenu(hWnd, FALSE);
	DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
	DeleteMenu(hMenu, SC_SIZE, MF_BYCOMMAND);

	// Add menu for starting a new game, exiting the game, looking at saved scores.
	HMENU hMenubar = CreateMenu();
	HMENU hFileMenu = CreatePopupMenu();
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hFileMenu, L"&Menu");
	SetMenu(hWnd, hMenubar);

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

	GameGraphics Graphics = {};
	Graphics.Buffer = BitmapBuffer;
	Graphics.Width = GameWindowWidth;
	Graphics.Height = GameWindowHeight;

	GameSession Session = {};
	GameInitialize(&Graphics, &Session);
	GameStart(&Graphics, &Session);
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

			GameKey TempKey = None;
			if ((Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN))
			{
				TempKey = Down;
			}
			if ((Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT))
			{
				TempKey = Left;
			}
			if ((Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) {
				TempKey = Right;
			}
			if ((Pad->wButtons & XINPUT_GAMEPAD_B))
			{
				TempKey = Rotate;
			}
			if ((Pad->wButtons & XINPUT_GAMEPAD_A))
			{
				TempKey = Drop;
			}

			if (TempKey != ControllerKey)
			{
				ControllerKey = TempKey;
				Key = TempKey;
			}
		}

		// Game Render Here
		GameUpdate(&Graphics, &Session, Key);
		// Clear Key
		Key = None;


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
			MessageBoxW(hWnd, L"Game over!", L"Block", MB_OK | MB_ICONWARNING);
		}
	}

	// Cleanup Render Buffer
	if (BitmapBuffer != NULL)
		VirtualFree(BitmapBuffer, 0, MEM_RELEASE);

	// Unregister the Window Class
	UnregisterClassW(lpszClassName, hInstance);

	return 0;
}