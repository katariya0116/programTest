#pragma once

#ifndef __RENDERER_H__
#define __RENDERER_H__

class CVertexBuff;
class CIndexBuff;
class CPipeline;

/// ポリゴン描画用パラメータ
struct RENDER_DRAW_POLYGON_PARAM
{
	const CVertexBuff* vtxBuff = nullptr;	//< 頂点バッファ
	const CIndexBuff* indexBuff = nullptr;	//< インデックスバッファ
	u32 drawType = 0;						//< 描画タイプ
};

/// レンダラーインターフェース
class IRenderer
{
public:
	using Color = LIB_KATA::Color;

public:
	virtual void Finalize() = 0;

public:
	virtual void BeginDraw() = 0;
	virtual void EndDraw() = 0;

public:
	// パイプライン設定
	virtual void SetPipline(const CPipeline* _pipline) = 0;

	// ポリゴン描画
	virtual void DrawPolygon(const RENDER_DRAW_POLYGON_PARAM& _drawParam) = 0;

public:
	virtual const c8* GetName() const { return nullptr; }
};

/// デバイス取得型テンプレートクラス
template<class TDevice, class INIT_RENDER_PARAM>
class TRenderer : public IRenderer
{
public:
	using INIT_PARAM = INIT_RENDER_PARAM;
	using DEVICE = TDevice;

public:
	virtual b8 Initialize(const INIT_PARAM& _data) { return false; }
	virtual void Finalize() override { }

public:
	virtual void BeginDraw() { }
	virtual void EndDraw() { }

public:
	// 
	virtual void SetPipline(const CPipeline* _pipline) {};

	// 
	virtual void DrawPolygon(const RENDER_DRAW_POLYGON_PARAM& _drawParam) {}

public:
	virtual DEVICE* GetDevice() = 0;
};
#endif#pragma once
