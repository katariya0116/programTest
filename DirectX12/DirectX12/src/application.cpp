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

	static 	CWindow			s_window;
	static	CVertexBuff		s_vertexBuff;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Vertex
{
	f32 Position[3];     //!< �ʒu���W�ł�.
	f32 Normal[3];       //!< �@���x�N�g���ł�.
	f32 TexCoord[2];     //!< �e�N�X�`�����W�ł�.
	f32 Color[4];        //!< ���_�J���[�ł�.
};

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

	// �E�B���h�E�̐���
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

	// �����_���[�̏�����
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

	// �A�v���P�[�V�����̏�����
	ret = InitializeApp();
	if (!ret)
	{
		return ret;
	}

	return ret;
}

void CApplication::Loop()
{
	// �Q�[�����[�v
	while (1)
	{
		b8 windowUpdate = s_window.Update();
		if (!windowUpdate) { break; }

		// �`��J�n
		CRenderUtility::Begin();
		{
			RENDER_DRAW_POLYGON_PARAM param;
			param.drawType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			param.vtxBuff = &s_vertexBuff;
			CRenderUtility::DrawPolygon(param);
		}
		// �`��I��
		CRenderUtility::End();
	}
}

void CApplication::Finalize()
{
	FinalizeApp();

	CRenderUtility::Finalize();

	// �I������
	s_window.Finalize();
}

b8 CApplication::InitializeApp()
{
	CRendererDx12* render = SCast<CRendererDx12*>(CRenderUtility::GetRenderer());
	render->CreateRootSignature();
	render->CreatePipeline();
	render->CreateConstantBuff();

	// ���_�f�[�^.
	Vertex vertices[] = {
		{ { 0.0f,  1.0f, 0.0f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 1.0f, -1.0f, 0.0f },{ 0.0f, 0.0f, -1.0f },{ 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -1.0f, -1.0f, 0.0f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
	};
	CVertexBuff::CREATE_VTX_BUFF_PARAM param;
	param._buff = vertices;
	param.stride = sizeof(Vertex);
	param.size = sizeof(vertices);
	param.vertexNum = param.size / param.stride;
	s_vertexBuff.Create(param);

	return true;
}

void CApplication::FinalizeApp()
{
	CRendererDx12* render = SCast<CRendererDx12*>(CRenderUtility::GetRenderer());
	render->ReleaseConstantBuff();
	render->ReleasePipeline();
	render->ReleaseRootSignature();

	s_vertexBuff.Release();
}