#include <windows.h>
#include "block.h"

static GameState GameStatus = {};

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
			MoveBlock(&GameStatus, 1, 0);
		}
		else if (VKCode == 'A')
		{
			MoveBlock(&GameStatus, 0, -1);
		}
		else if (VKCode == 'D')
		{
			MoveBlock(&GameStatus, 0, 1);
		}
		else if (VKCode == 'J')
		{
			RotateBlock(&GameStatus);
		}
		else if (VKCode == VK_SPACE)
		{
			DropBlock(&GameStatus);
		}
	} break;
	case WM_TIMER:
	{
		MoveBlock(&GameStatus, 1, 0);
	} break;
	case WM_CLOSE:
	{
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
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hFileMenu, L"&File");
	SetMenu(hWnd, hMenubar);

	// Create Render Buffer
	void* BitmapMemory = VirtualAlloc(0, sizeof(uint32_t) * GameWindowWidth * GameWindowHeight, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (BitmapMemory == NULL)
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

	GraphicsInfo GameGraphicsInfo = {};
	GameGraphicsInfo.BitmapMemory = BitmapMemory;
	GameGraphicsInfo.Width = GameWindowWidth;
	GameGraphicsInfo.Height = GameWindowHeight;


	GameInitialize(&GameGraphicsInfo, &GameStatus);

	const int ID_TIMER = 1;
	if (SetTimer(hWnd, ID_TIMER, 1000, NULL) == 0)
		MessageBoxW(hWnd, L"Could not SetTimer()!", L"Error", MB_OK | MB_ICONEXCLAMATION);

	// Main event loop
	MSG msg;
	while (GetMessageW(&msg, hWnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		// Game Render Here
		GameUpdate(&GameGraphicsInfo, &GameStatus);

		HDC hdc = GetDC(hWnd);
		StretchDIBits(hdc,
			0, 0, GameWindowWidth, GameWindowHeight, // TODO: ~ window resizing
			0, 0, GameWindowWidth, GameWindowHeight,
			BitmapMemory,
			&BitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY
		);
		ReleaseDC(hWnd, hdc);
	}

	// Cleanup Timer
	KillTimer(hWnd, ID_TIMER);

	// Cleanup Render Buffer
	if (BitmapMemory != NULL)
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);

	// Unregister the Window Class
	UnregisterClassW(lpszClassName, hInstance);

	return 0;
}