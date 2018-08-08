#include "application.h"

#include "window/window.h"
#include "renderer/renderer_utility.h"
#include "renderer/dx12/renderer_dx12.h"

#include "math/vector.h"
namespace 
{
	constexpr s32 WINDOW_WIDTH = 640;
	constexpr s32 WINDOW_HEIGHT = 480;
	constexpr c8 WND_CLASS_NAME[] = "DirectX12";

	static 	CWindow s_window;
}

using namespace LIB_KATA;

CApplication::CApplication()
{
}

CApplication::~CApplication()
{
}

b8 CApplication::Run()
{
	b8 ret = Initialize();
	if (ret)
	{
		Loop();
	}
	Finalize();

	return ret;
}

b8 CApplication::Initialize()
{
	b8 ret = false;

	// ウィンドウの生成
	CWindow::INIT_PARAM param;
	param.width = WINDOW_WIDTH;
	param.height = WINDOW_HEIGHT;
	param.className = WND_CLASS_NAME;
	param.windowName = WND_CLASS_NAME;
	ret = s_window.Initialize(param);
	if (!ret)
	{
		return ret;
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
		return ret;
	}

	// アプリケーションの初期化
	ret = InitializeApp();
	if (!ret)
	{
		return ret;
	}

	return ret;
}

void CApplication::Loop()
{
	// ゲームループ
	while (1)
	{
		b8 windowUpdate = s_window.Update();
		if (!windowUpdate) { break; }

		// 描画開始
		CRenderUtility::Begin();

		// 描画終了
		CRenderUtility::End();

	}
}

void CApplication::Finalize()
{
	FinalizeApp();

	CRenderUtility::Finalize();

	// 終了処理
	s_window.Finalize();
}

b8 CApplication::InitializeApp()
{
	CRendererDx12* render = SCast<CRendererDx12*>(CRenderUtility::GetRenderer());
	render->CreateRootSignature();
	render->CreatePipeline();
	render->CreateVertexBuff();
	render->CreateConstantBuff();

	return true;
}

void CApplication::FinalizeApp()
{
	CRendererDx12* render = SCast<CRendererDx12*>(CRenderUtility::GetRenderer());
	render->ReleaseConstantBuff();
	render->ReleaseVertexBuff();
	render->ReleasePipeline();
	render->ReleaseRootSignature();
}