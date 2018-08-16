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
	f32 Position[3];     //!< 位置座標です.
	f32 Normal[3];       //!< 法線ベクトルです.
	f32 TexCoord[2];     //!< テクスチャ座標です.
	f32 Color[4];        //!< 頂点カラーです.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// 定数バッファ
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

	static ResConstantBuffer s_constantBufferData;	//コンスタントバッファ
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
	
		Update();
		Draw();
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
	// パイプラインの生成
	CPipeline::INIT_PARAM ppParam;
	s_pipline.Create(ppParam);

	// 頂点データ.
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

	// アスペクト比算出.
	f32 aspectRatio = SCast<f32>(WINDOW_WIDTH) / SCast<f32>(WINDOW_HEIGHT);

	// 定数バッファデータの設定.
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
	// 回転角を増やす.
	static f32 rot = 0.0f;
	rot = (0.01f / 0.5f);

	// ワールド行列を更新.
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
	// 描画開始
	CRenderUtility::Begin();
	{
		// パイプラインの設定
		CRenderUtility::SetPipeline(&s_pipline);

		// パイプラインのバッファを設定
		s_pipline.SetConstantBuff(0, &s_constantBufferData, sizeof(ResConstantBuffer));

		RENDER_DRAW_POLYGON_PARAM param;
		param.drawType = 4;
		param.vtxBuff = &s_vertexBuff;
		CRenderUtility::DrawPolygon(param);
	}
	// 描画終了
	CRenderUtility::End();
}
