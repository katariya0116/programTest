#pragma once

#ifndef __RENDERER_H__
#define __RENDERER_H__

class CVertexBuff;
class CIndexBuff;
class CPipeline;

/// �|���S���`��p�p�����[�^
struct RENDER_DRAW_POLYGON_PARAM
{
	const CVertexBuff* vtxBuff = nullptr;	//< ���_�o�b�t�@
	const CIndexBuff* indexBuff = nullptr;	//< �C���f�b�N�X�o�b�t�@
	u32 drawType = 0;						//< �`��^�C�v
};

/// �����_���[�C���^�[�t�F�[�X
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
	// �p�C�v���C���ݒ�
	virtual void SetPipline(const CPipeline* _pipline) = 0;

	// �|���S���`��
	virtual void DrawPolygon(const RENDER_DRAW_POLYGON_PARAM& _drawParam) = 0;

public:
	virtual const c8* GetName() const { return nullptr; }
};

/// �f�o�C�X�擾�^�e���v���[�g�N���X
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
