#pragma once

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <windows.h>

struct INIT_RENDERER_DATA
{
	HWND	hWnd;
	s32		width;
	s32		height;
};

class CRenderUtility
{
public:
	static b8 Initialize(const INIT_RENDERER_DATA& _data);

	static void Finalize();

public:
	static void Begin();

	static void End();
};

#endif