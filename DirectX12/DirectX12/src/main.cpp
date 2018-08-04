#include "type.h"
#include "window.h"
#include "renderer/renderer_utility.h"
#include "math/vector.h"
#include "math/matrix.h"

namespace {
	constexpr s32 WINDOW_WIDTH = 640;
	constexpr s32 WINDOW_HEIGHT = 480;
	constexpr c8 WND_CLASS_NAME[] = "DirectX12";
}

using namespace LIB_KATA_NAMESPACE;

s32 WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, s32 nShowCmd)
{
	b8 ret = false;

	// ウィンドウの生成
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
	
	// レンダラーの初期化
	INIT_RENDER_PARAM initData;
	initData.type = RENDERER_TYPE::DX12;
	initData.width = WINDOW_WIDTH;
	initData.height = WINDOW_HEIGHT;
	initData.windowMode = true;
	ret = CRenderUtility::Initialize(initData);
	if (!ret)
	{
		return -1;
	}

	// ゲームループ
	while (window.Update()) 
	{
		// 描画開始
		CRenderUtility::Begin();

		// 描画終了
		CRenderUtility::End();
	}

	CRenderUtility::Finalize();

	// 終了処理
	window.Finalize();

	return 0;
}
