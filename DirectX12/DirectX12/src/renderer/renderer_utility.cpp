#include "renderer/renderer_utility.h"
#include "renderer/render.h"
#include "renderer/dx12/renderer_dx12.h"

//============================================================================================================
//
//============================================================================================================
static IRenderer* s_inst = nullptr;

b8 CRenderUtility::Initialize(const INIT_RENDER_PARAM& _param)
{
	b8 ret = false;
	switch (_param.type)
	{
	case RENDERER_TYPE::DX12:
	{
		CRendererDx12* dxRender = new CRendererDx12();
		Assert(dxRender);
		if (dxRender == nullptr) { return false; }

		CRendererDx12::INIT_PARAM param;
		param.hwnd =::GetActiveWindow();
		param.width = _param.width;
		param.height = _param.height;
		ret = dxRender->Initialize(param);
		s_inst = dxRender;
	}
	break;
	default:
		Assert(0);
		return false;
	}

	return ret;
}

void CRenderUtility::Finalize()
{
	if (s_inst)
	{
		s_inst->Finalize();
		delete s_inst;
		s_inst = nullptr;

	}
}

void CRenderUtility::Begin()
{
	Assert(s_inst);
	s_inst->BeginDraw(IRenderer::Color(0.0f, 1.0f, 0.0f, 1.0f));
}

void CRenderUtility::End()
{
	Assert(s_inst);
	s_inst->EndDraw();
}


IRenderer* CRenderUtility::GetRenderer()
{
	return s_inst;
}
