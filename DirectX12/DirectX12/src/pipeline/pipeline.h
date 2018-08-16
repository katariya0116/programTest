#pragma once

struct	ID3D12PipelineState;
struct	ID3D12RootSignature;
struct	ID3D12DescriptorHeap;
class	CConstantBuff;

class CPipeline
{
public:
	struct INIT_PARAM
	{
		// パイプラインの情報を載せる構造体（おそらくリソース周り必要だろうなあ…）
	};

public:
	CPipeline();
	~CPipeline();

public:
	b8 Create(const INIT_PARAM& _param);
	void Release();

public:
	ID3D12PipelineState*	GetPipelineState() const { return m_pipelineState; }
	ID3D12RootSignature*	GetRootSignature() const { return m_rootSignature; }
	ID3D12DescriptorHeap**	GetConstBuffDHList() const { return m_constantBuffDsHeap; }
	u32 GetConstBuffNum() const { return m_constBuffNum; }

public:
	void SetConstantBuff(u32 _idx, const void* _buff, size_t _buffSize);

private:
	b8 __InitializeRootSignature();
	b8 __InitializePipeline();

private:
	// パイプライン情報
	ID3D12PipelineState*	m_pipelineState;
	ID3D12RootSignature*	m_rootSignature;
	CConstantBuff*			m_constBuffList;
	u32						m_constBuffNum;
	ID3D12DescriptorHeap**	m_constantBuffDsHeap;
};