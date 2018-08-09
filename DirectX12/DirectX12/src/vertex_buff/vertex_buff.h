#pragma once

#include <d3d12.h>

/// ���_�o�b�t�@
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
	// ���_���̎擾
	u32 GetVertexNum() const { return m_vertexNum; }

public:
	// ���_�o�b�t�@�r���[�̎擾
	const D3D12_VERTEX_BUFFER_VIEW* GetVertexView() const { return &m_vertexBufferView; }

private:
	ID3D12Resource*             m_vertexBuffer;		// ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW    m_vertexBufferView;	// ���_�o�b�t�@�̃|�C���^��ʒu�ƃT�C�Y
	u32							m_vertexNum;		// ���_��
};

/// �C���f�b�N�X�o�b�t�@
class CIndexBuff
{
public:
	CIndexBuff();
	~CIndexBuff();

};