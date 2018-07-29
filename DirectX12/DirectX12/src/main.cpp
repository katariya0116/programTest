#include "type.h"
#include "window.h"
#include "renderer_dx12.h"

namespace {
	constexpr s32 WINDOW_WIDTH = 640;
	constexpr s32 WINDOW_HEIGHT = 480;
	constexpr c8 WND_CLASS_NAME[] = "DirectX12";
}

s32 WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, s32 nShowCmd)
{
	b8 ret = false;

	// �E�B���h�E�̐���
	CWindow window;
	CWindow::INIT_PARAM param;
	param.hInst = hInst;
	param.width = WINDOW_WIDTH;
	param.height = WINDOW_HEIGHT;
	param.className = WND_CLASS_NAME;
	param.windowName = WND_CLASS_NAME;
	ret = window.Initialize(param);
	if (!ret)
	{
		return -1;
	}
	
	// �����_���[�̏�����
	INIT_RENDERER_DATA initData;
	initData.hWnd = window.GetHwnd();
	initData.width = WINDOW_WIDTH;
	initData.height = WINDOW_HEIGHT;
	ret = CRenderUtility::Initialize(initData);
	if (!ret)
	{
		return -1;
	}

	// �Q�[�����[�v
	while (window.Update()) 
	{
		// �`��J�n
		CRenderUtility::Begin();

		// �`��I��
		CRenderUtility::End();
	}

	CRenderUtility::Finalize();

	// �I������
	window.Finalize();

	return 0;
}
