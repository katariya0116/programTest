#include "window.h"

CWindow::CWindow()
{
}

CWindow::~CWindow()
{
}

b8 CWindow::Initialize(const CWindow::INIT_PARAM& _initParam)
{
	m_className = _initParam.className;

	WNDCLASSEX	wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _initParam.hInst;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _initParam.className;
	wc.hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIconSm = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	if (RegisterClassEx(&wc) == 0) 
	{
		return false;
	}

	m_hwnd = CreateWindowEx(
		WS_EX_COMPOSITED,
		_initParam.className,
		_initParam.windowName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		_initParam.width,
		_initParam.height,
		NULL,
		NULL,
		_initParam.hInst,
		NULL);

	{
		// 画面の幅と高さを取る
		s32 screen_width = GetSystemMetrics(SM_CXSCREEN);
		s32 screen_height = GetSystemMetrics(SM_CYSCREEN);
		
		// ウィンドウの値をとる
		RECT rect{};
		GetClientRect(m_hwnd, &rect);

		//デフォルトはウインドウを画面中央に持ってくる
		s32 windowPosX = (screen_width / 2) - ((_initParam.width + (_initParam.width - rect.right)) / 2);
		s32 windowPosY = (screen_height / 2) - ((_initParam.height + (_initParam.height - rect.bottom)) / 2);

		// ウィンドウの幅（実画面に出せるサイズを取る）
		s32 windowWidth = _initParam.width + (_initParam.width - rect.right);
		s32 windowHeight = _initParam.height + (_initParam.height - rect.bottom);

		MoveWindow(
			m_hwnd,
			windowPosX,	windowPosY,
			windowWidth, windowHeight,
			TRUE);
	}

	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);

	return true;
}

b8 CWindow::Update()
{
	MSG msg{};
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) { return false; }
	}
	return true;
}

void CWindow::Finalize()
{
	UnregisterClass(m_className, NULL);
	m_hwnd = NULL;
}


LRESULT CALLBACK CWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) {
	// ウィンドウ破壊
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return -1;
	}

	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}