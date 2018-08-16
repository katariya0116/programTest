#include "pipeline/pipeline.h"
#include "shader/shader.h"
#include "renderer/dx12/renderer_dx12.h"
#include "renderer/renderer_utility.h"
#include "vertex_buff/vertex_buff.h"

#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>

namespace 
{
	// ��Œu��������X�}�|
	template<class TClass>
	using WinShPtr = Microsoft::WRL::ComPtr<TClass>;

	// ���Ƃňړ��\��
	static CShader			s_vsShader;
	static CShader			s_psShader;

};

CPipeline::CPipeline()
	: m_pipelineState(nullptr)
	, m_rootSignature(nullptr)
{
}

CPipeline::~CPipeline()
{
}

b8 CPipeline::Create(const CPipeline::INIT_PARAM& _param)
{
	if (!__InitializeRootSignature())
	{
		return false;
	}

	if (!__InitializePipeline())
	{
		return false;
	}

	return true;
}

void CPipeline::Release()
{
	if (m_pipelineState)
	{
		m_pipelineState->Release();
		m_pipelineState = nullptr;
	}

	// �V�F�[�_�[�̃����[�X
	// (���Ƃňړ��\��)
	s_vsShader.Release();
	s_psShader.Release();

	if (m_constBuffList)
	{
		for (u32 idx = 0; idx < m_constBuffNum; ++idx)
		{
			m_constBuffList[idx].Release();
		}
		delete[] m_constBuffList;
		m_constBuffList = nullptr;
	}

	if (m_constantBuffDsHeap)
	{
		free(m_constantBuffDsHeap);
		m_constantBuffDsHeap = nullptr;
	}

	if (m_rootSignature)
	{
		m_rootSignature->Release();
		m_rootSignature = nullptr;
	}
}

void CPipeline::SetConstantBuff(u32 _idx, const void* _buff, size_t _buffSize)
{
	m_constBuffList[_idx].CopyConstantBuff(_buff, _buffSize);
}

b8 CPipeline::__InitializeRootSignature()
{
	CRendererDx12* render = SCast<CRendererDx12*>(CRenderUtility::GetRenderer());
	CRendererDx12::DEVICE* device = render->GetDevice();

	HRESULT hr{};

	// �f�B�X�N���v�^�����W�̐ݒ�i�萔�o�b�t�@�������ݗp�̃����W����j
	D3D12_DESCRIPTOR_RANGE range;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// ����DescriptorRange�͒萔�o�b�t�@
	range.NumDescriptors = 1;							// Descriptor�͈��
	range.BaseShaderRegister = 0;						// �V�F�[�_���̊J�n�C���f�b�N�X��0��
	range.RegisterSpace = 0;							// TODO: SM5.1�����space�����ǁA�ǂ��������̂��悭�킩��Ȃ�
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // TODO: �Ƃ肠����-1�����Ă�����OK�H

																					// ���[�g�p�����[�^�̐ݒ�.
	D3D12_ROOT_PARAMETER		root_parameters;
	root_parameters.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // �����DescriptorTable�ł���
	root_parameters.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;			// �p�����[�^�͒��_�V�F�[�_�[�݂̂���m�F�\
	root_parameters.DescriptorTable.NumDescriptorRanges = 1;					// DescriptorRange�̐�
	root_parameters.DescriptorTable.pDescriptorRanges = &range;					// DescriptorRange�̐擪�A�h���X
	//root_parameters.Constants.Num32BitValues = 4;								// D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS�i32�r�b�g�̒萔�j�������蓖�Ă�����
																				// ���[�g�V�O�l�`���[�̒�`�ݒ�
	D3D12_ROOT_SIGNATURE_DESC	root_signature_desc{};
	root_signature_desc.NumParameters = 1;
	root_signature_desc.pParameters = &root_parameters;
	root_signature_desc.NumStaticSamplers = 0;
	root_signature_desc.pStaticSamplers = nullptr;
	root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	WinShPtr<ID3DBlob> pSignature{};
	WinShPtr<ID3DBlob> pError{};

	// ��`���烋�[�g�V�O�l�`���[�̃V���A���C�Y
	hr = D3D12SerializeRootSignature(&root_signature_desc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		pSignature.GetAddressOf(),
		pError.GetAddressOf());
	if (FAILED(hr))
	{
		PRINT_ERROR("D3D12SerializeRootSignature Error");
		return false;
	}

	// ���[�g�V�O�l�`���[�̐���
	hr = device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	if (FAILED(hr))
	{
		PRINT_ERROR("CreateRootSignature Error");
		return false;
	}

	return true;

}

b8 CPipeline::__InitializePipeline()
{
	CRendererDx12* render = SCast<CRendererDx12*>(CRenderUtility::GetRenderer());
	CRendererDx12::DEVICE* device = render->GetDevice();

	// ���ŃV�F�[�_�[�����ō���Ă邯�ǌ�ŕ��U�\��
	// ����������ӂ��������\���̂Ɉړ����邱�ƂɂȂ�Ǝv��
#if 1
	// ���_�V�F�[�_�̃t�@�C���p�X������.
	// �R���p�C���ςݒ��_�V�F�[�_��ǂݍ���.
	HRESULT hr = FAILED_ACCESS_ACE_FLAG;
	CShader::INIT_PARAM vsParam;
	vsParam.shaderPath = L"D:\\program\\programTest\\DirectX12\\DirectX12\\bin\\Debug\\x64\\shader_vs.cso";
	vsParam.shaderType = SHADER_TYPE::VS;
	if (!s_vsShader.Create(vsParam))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_�̃t�@�C���p�X������.
	// �R���p�C���ς݃s�N�Z���V�F�[�_��ǂݍ���.
	CShader::INIT_PARAM psParam;
	psParam.shaderPath = L"D:\\program\\programTest\\DirectX12\\DirectX12\\bin\\Debug\\x64\\shader_ps.cso";
	psParam.shaderType = SHADER_TYPE::PS;
	if (!s_psShader.Create(psParam))
	{
		return false;
	}

	// ���̓��C�A�E�g�̐ݒ�.
	D3D12_INPUT_ELEMENT_DESC inputElements[] = {
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "VTX_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// �R���X�^���g�o�b�t�@�̐���
	m_constBuffNum = 1;
	m_constBuffList = new CConstantBuff[m_constBuffNum];
	if (m_constBuffList)
	{
		for (u32 idx = 0; idx < m_constBuffNum;++idx)
		{
			CConstantBuff::CREATE_PARAM param;
			param.buffSize = 256;	// ���ŌŒ�i������ӑ����V�F�[�_�[���ɏ�����������j
			m_constBuffList[idx].Create(param);
		}
	}
	m_constantBuffDsHeap = SCast<ID3D12DescriptorHeap**>(malloc(sizeof(ID3D12DescriptorHeap*) * m_constBuffNum));
	if (m_constantBuffDsHeap)
	{
		for (u32 idx = 0; idx < m_constBuffNum; ++idx)
		{
			m_constantBuffDsHeap[idx] = m_constBuffList[idx].GetDescriptorHeap();
		}
	}

#endif


	// ���X�^���C�U�[�X�e�[�g�̐ݒ�.
	D3D12_RASTERIZER_DESC descRS;
	descRS.FillMode = D3D12_FILL_MODE_SOLID;
	descRS.CullMode = D3D12_CULL_MODE_NONE;
	descRS.FrontCounterClockwise = FALSE;
	descRS.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	descRS.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	descRS.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	descRS.DepthClipEnable = TRUE;
	descRS.MultisampleEnable = FALSE;
	descRS.AntialiasedLineEnable = FALSE;
	descRS.ForcedSampleCount = 0;
	descRS.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�.
	D3D12_RENDER_TARGET_BLEND_DESC descRTBS =
	{
		FALSE, FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	};

	// �u�����h�X�e�[�g�̐ݒ�.
	D3D12_BLEND_DESC descBS;
	descBS.AlphaToCoverageEnable = FALSE;
	descBS.IndependentBlendEnable = FALSE;
	for (UINT i = 0; i<D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		descBS.RenderTarget[i] = descRTBS;
	}

	// �p�C�v���C���X�e�[�g�̐ݒ�.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout = { inputElements, _countof(inputElements) };
	desc.pRootSignature = m_rootSignature;
	desc.VS = { RCast<UINT8*>(s_vsShader.GetBuffer()), s_vsShader.GetBufferSize() };
	desc.PS = { RCast<UINT8*>(s_psShader.GetBuffer()), s_psShader.GetBufferSize() };
	desc.RasterizerState = descRS;
	desc.BlendState = descBS;
	desc.DepthStencilState.DepthEnable = FALSE;
	desc.DepthStencilState.StencilEnable = FALSE;
	desc.SampleMask = UINT_MAX;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;

	// �p�C�v���C���X�e�[�g�𐶐�.
	hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : ID3D12Device::CreateGraphicsPipelineState() Failed.");
		return false;
	}
	return true;

}