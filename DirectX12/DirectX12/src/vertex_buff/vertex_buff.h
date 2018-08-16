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

/// �萔�o�b�t�@
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
	// �萔�o�b�t�@�ɌŒ�
	void CopyConstantBuff(const void* _buff, size_t buffSize);

public:
	// �q�[�v�̏������݂��s��
	ID3D12DescriptorHeap* GetDescriptorHeap();

private:
	ID3D12Resource*             m_constantBuffer;	// �萔�o�b�t�@
	ID3D12DescriptorHeap*       m_cbvHeap;			// �萔�o�b�t�@�������݃N���X
	u8*                         m_cbvDataBegin;		// CBV�̏������݃������J�n�ʒu
};