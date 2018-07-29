#pragma once

#ifndef __RENDERER_H__
#define __RENDERER_H__

/// レンダラーインターフェース
class IRenderer
{
public:
	using Color = LIB_KATA_NAMESPACE::Color;

public:
	virtual void Finalize() = 0;

public:
	virtual void BeginDraw(const Color& _clearColor) = 0;
	virtual void EndDraw() = 0;

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
	virtual void BeginDraw(const Color& _clearColor) { }
	virtual void EndDraw() { }

public:
	virtual DEVICE* GetDevice() = 0;
};
#endif#pragma once
