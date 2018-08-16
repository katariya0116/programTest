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
	// 後で置き換えるスマポ
	template<class TClass>
	using WinShPtr = Microsoft::WRL::ComPtr<TClass>;

	// あとで移動予定
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

	// シェーダーのリリース
	// (あとで移動予定)
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

	// ディスクリプタレンジの設定（定数バッファ書き込み用のレンジだよ）
	D3D12_DESCRIPTOR_RANGE range;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// このDescriptorRangeは定数バッファ
	range.NumDescriptors = 1;							// Descriptorは一つ
	range.BaseShaderRegister = 0;						// シェーダ側の開始インデックスは0番
	range.RegisterSpace = 0;							// TODO: SM5.1からのspaceだけど、どういうものかよくわからない
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // TODO: とりあえず-1を入れておけばOK？

																					// ルートパラメータの設定.
	D3D12_ROOT_PARAMETER		root_parameters;
	root_parameters.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // これはDescriptorTableである
	root_parameters.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;			// パラメータは頂点シェーダーのみから確認可能
	root_parameters.DescriptorTable.NumDescriptorRanges = 1;					// DescriptorRangeの数
	root_parameters.DescriptorTable.pDescriptorRanges = &range;					// DescriptorRangeの先頭アドレス
	//root_parameters.Constants.Num32BitValues = 4;								// D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS（32ビットの定数）を何個割り当てたいか
																				// ルートシグネチャーの定義設定
	D3D12_ROOT_SIGNATURE_DESC	root_signature_desc{};
	root_signature_desc.NumParameters = 1;
	root_signature_desc.pParameters = &root_parameters;
	root_signature_desc.NumStaticSamplers = 0;
	root_signature_desc.pStaticSamplers = nullptr;
	root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	WinShPtr<ID3DBlob> pSignature{};
	WinShPtr<ID3DBlob> pError{};

	// 定義からルートシグネチャーのシリアライズ
	hr = D3D12SerializeRootSignature(&root_signature_desc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		pSignature.GetAddressOf(),
		pError.GetAddressOf());
	if (FAILED(hr))
	{
		PRINT_ERROR("D3D12SerializeRootSignature Error");
		return false;
	}

	// ルートシグネチャーの生成
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

	// 仮でシェーダー内部で作ってるけど後で分散予定
	// 多分ここら辺を初期化構造体に移動することになると思う
#if 1
	// 頂点シェーダのファイルパスを検索.
	// コンパイル済み頂点シェーダを読み込む.
	HRESULT hr = FAILED_ACCESS_ACE_FLAG;
	CShader::INIT_PARAM vsParam;
	vsParam.shaderPath = L"D:\\program\\programTest\\DirectX12\\DirectX12\\bin\\Debug\\x64\\shader_vs.cso";
	vsParam.shaderType = SHADER_TYPE::VS;
	if (!s_vsShader.Create(vsParam))
	{
		return false;
	}

	// ピクセルシェーダのファイルパスを検索.
	// コンパイル済みピクセルシェーダを読み込む.
	CShader::INIT_PARAM psParam;
	psParam.shaderPath = L"D:\\program\\programTest\\DirectX12\\DirectX12\\bin\\Debug\\x64\\shader_ps.cso";
	psParam.shaderType = SHADER_TYPE::PS;
	if (!s_psShader.Create(psParam))
	{
		return false;
	}

	// 入力レイアウトの設定.
	D3D12_INPUT_ELEMENT_DESC inputElements[] = {
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "VTX_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// コンスタントバッファの生成
	m_constBuffNum = 1;
	m_constBuffList = new CConstantBuff[m_constBuffNum];
	if (m_constBuffList)
	{
		for (u32 idx = 0; idx < m_constBuffNum;++idx)
		{
			CConstantBuff::CREATE_PARAM param;
			param.buffSize = 256;	// 仮で固定（ここら辺多分シェーダー側に情報を持たせる）
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


	// ラスタライザーステートの設定.
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

	// レンダーターゲットのブレンド設定.
	D3D12_RENDER_TARGET_BLEND_DESC descRTBS =
	{
		FALSE, FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	};

	// ブレンドステートの設定.
	D3D12_BLEND_DESC descBS;
	descBS.AlphaToCoverageEnable = FALSE;
	descBS.IndependentBlendEnable = FALSE;
	for (UINT i = 0; i<D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		descBS.RenderTarget[i] = descRTBS;
	}

	// パイプラインステートの設定.
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

	// パイプラインステートを生成.
	hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : ID3D12Device::CreateGraphicsPipelineState() Failed.");
		return false;
	}
	return true;

}