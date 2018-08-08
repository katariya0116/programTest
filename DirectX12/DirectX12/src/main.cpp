#include "application.h"

s32 WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, s32 nShowCmd)
{
	CApplication app;
	b8 ret = app.Run();
	return ret ? 0 : -1;
}
