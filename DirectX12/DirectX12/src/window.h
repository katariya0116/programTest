#pragma once

#include <windows.h>

class CWindow
{
public:
	struct INIT_PARAM
	{
		const c8* windowName = NULL;
		const c8* className = NULL;
		HINSTANCE hInst = 0;
		s32 width = 0;
		s32 height = 0;
		s32 posX = -1;
		s32 posY = -1;
	};
public:
	CWindow();
	virtual ~CWindow();

public:
	/// 初期化
	b8 Initialize(const INIT_PARAM& _param);

	/// ウィンドウ更新
	b8 Update();

	/// 終了処理
	void Finalize();

public:
	///ウィンドウハンドル取得
	HWND GetHwnd() const { return m_hwnd; };

#ifdef _DEBUG
public:
	void ShowConsoleWin();
	void CloseConsoleWin();
#endif

public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hwnd;
	const c8* m_className;

#ifdef _DEBUG
	s32 m_hConsole;
#endif

};
