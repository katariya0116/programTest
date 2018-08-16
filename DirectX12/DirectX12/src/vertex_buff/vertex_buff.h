#pragma once

#include <d3d12.h>

/// 頂点バッファ
class CVertexBuff
{
public:
	struct CREATE_VTX_BUFF_PARAM
	{
		const void* _buff = nullptr;
		u32 size = 0;
		u32 stride = 0;
		u32 vertexNum = 0;
	};
public:
	CVertexBuff();
	~CVertexBuff();

public:
	b8 Create(const CREATE_VTX_BUFF_PARAM& _param);
	void Release();

public:
	// 頂点数の取得
	u32 GetVertexNum() const { return m_vertexNum; }

public:
	// 頂点バッファビューの取得
	const D3D12_VERTEX_BUFFER_VIEW* GetVertexView() const { return &m_vertexBufferView; }

private:
	ID3D12Resource*             m_vertexBuffer;		// 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW    m_vertexBufferView;	// 頂点バッファのポインタ一位置とサイズ
	u32							m_vertexNum;		// 頂点数
};

/// インデックスバッファ
class CIndexBuff
{
public:
	CIndexBuff();
	~CIndexBuff();

};

/// 定数バッファ
class CConstantBuff
{
public:
	struct CREATE_PARAM
	{
		u32	buffSize;
	};
public:
	CConstantBuff();
	~CConstantBuff();

public:
	b8 Create(const CREATE_PARAM& _param);
	void Release();

public:
	// 定数バッファに固定
	void CopyConstantBuff(const void* _buff, size_t buffSize);

public:
	// ヒープの書き込みを行う
	ID3D12DescriptorHeap* GetDescriptorHeap();

private:
	ID3D12Resource*             m_constantBuffer;	// 定数バッファ
	ID3D12DescriptorHeap*       m_cbvHeap;			// 定数バッファ書き込みクラス
	u8*                         m_cbvDataBegin;		// CBVの書き込みメモリ開始位置
};