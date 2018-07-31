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
	/// ������
	b8 Initialize(const INIT_PARAM& _param);

	/// �E�B���h�E�X�V
	b8 Update();

	/// �I������
	void Finalize();

public:
	///�E�B���h�E�n���h���擾
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
