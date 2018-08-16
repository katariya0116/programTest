#include "application.h"

#include "window/window.h"
#include "renderer/renderer_utility.h"

#include "renderer/render.h"
#include "pipeline/pipeline.h"
#include "vertex_buff/vertex_buff.h"
#include "math/matrix.h"

#include "math/vector.h"


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

///////////////////////////////////////////////////////////////////////////////////////////////////
// �萔�o�b�t�@
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LIB_ALIGNED(256) ResConstantBuffer
{
	LIB_KATA::CMatrix44   World;
	LIB_KATA::CMatrix44   View;
	LIB_KATA::CMatrix44   Proj;
};

namespace 
{
	constexpr s32 WINDOW_WIDTH = 640;
	constexpr s32 WINDOW_HEIGHT = 480;
	constexpr c8 WND_CLASS_NAME[] = "DirectX12";

	static 	CWindow			s_window;
	static	CVertexBuff		s_vertexBuff;
	static	CPipeline		s_pipline;

	static ResConstantBuffer s_constantBufferData;	//�R���X�^���g�o�b�t�@
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
	
		Update();
		Draw();
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
	// �p�C�v���C���̐���
	CPipeline::INIT_PARAM ppParam;
	s_pipline.Create(ppParam);

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

	// �A�X�y�N�g��Z�o.
	f32 aspectRatio = SCast<f32>(WINDOW_WIDTH) / SCast<f32>(WINDOW_HEIGHT);

	// �萔�o�b�t�@�f�[�^�̐ݒ�.
	LIB_KATA::MatrixUtil::Identity(s_constantBufferData.World);

	LIB_KATA::CMatrix44 view;
	LIB_KATA::MatrixUtil::CreateLookAt(LIB_KATA::CVector3(0.0f, 0.0f, 5.0f), LIB_KATA::CVector3(0.0f, 0.0f, 0.0f), LIB_KATA::CVector3(0.0f, 1.0f, 0.0f), view);
	s_constantBufferData.View = view;

	LIB_KATA::CMatrix44 proj;
	LIB_KATA::MatrixUtil::CreatePerspectiveFieldOfView(LIB_KATA::PI_DIV4, aspectRatio, 1.0f, 1000.0f, proj);
	s_constantBufferData.Proj = proj;

	return true;
}

void CApplication::FinalizeApp()
{
	s_vertexBuff.Release();

	s_pipline.Release();
}

void CApplication::Update()
{
	// ��]�p�𑝂₷.
	static f32 rot = 0.0f;
	rot = (0.01f / 0.5f);

	// ���[���h�s����X�V.
	LIB_KATA::CMatrix44 mtx;
	LIB_KATA::MatrixUtil::Identity(mtx);
	LIB_KATA::MatrixUtil::RotY(mtx, rot);

	LIB_KATA::CMatrix44 temp = mtx;
	//LIB_KATA::MatrixUtil::Transpose(temp, mtx);

	LIB_KATA::CMatrix44 retMtx;
	LIB_KATA::MatrixUtil::Identity(retMtx);
	LIB_KATA::MatrixUtil::Mul(retMtx, s_constantBufferData.World, temp);
	//LIB_KATA::MatrixUtil::Mul(retMtx, retMtx, s_constantBufferData.View);
	//LIB_KATA::MatrixUtil::Mul(retMtx, retMtx, s_constantBufferData.Proj);

	LIB_KATA::CVector4 ret[3];

	LIB_KATA::MatrixUtil::Transform(ret[0], retMtx, LIB_KATA::CVector4::Left + LIB_KATA::CVector4(0.0f, 0.0f, 0.0f, 1.0f));
	LIB_KATA::MatrixUtil::Transform(ret[1], retMtx, LIB_KATA::CVector4::Right + LIB_KATA::CVector4(0.0f, 0.0f, 0.0f, 1.0f));
	LIB_KATA::MatrixUtil::Transform(ret[2], retMtx, LIB_KATA::CVector4::Up + LIB_KATA::CVector4(0.0f, 0.0f, 0.0f, 1.0f));

	s_constantBufferData.World = retMtx;
}

void CApplication::Draw()
{
	// �`��J�n
	CRenderUtility::Begin();
	{
		// �p�C�v���C���̐ݒ�
		CRenderUtility::SetPipeline(&s_pipline);

		// �p�C�v���C���̃o�b�t�@��ݒ�
		s_pipline.SetConstantBuff(0, &s_constantBufferData, sizeof(ResConstantBuffer));

		RENDER_DRAW_POLYGON_PARAM param;
		param.drawType = 4;
		param.vtxBuff = &s_vertexBuff;
		CRenderUtility::DrawPolygon(param);
	}
	// �`��I��
	CRenderUtility::End();
}
