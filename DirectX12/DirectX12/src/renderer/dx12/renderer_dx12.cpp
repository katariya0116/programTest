#include "renderer_dx12.h"
#include "vertex_buff\vertex_buff.h"
#include "math/vector.h"
#include <dxgidebug.h>

// �萔�o�b�t�@�� 256 byte �A���C�����g�K�{.
struct LIB_ALIGNED(256) ResConstantBuffer
{
	LIB_KATA::CMatrix44   World;
	LIB_KATA::CMatrix44   View;
	LIB_KATA::CMatrix44   Proj;
};
static ResConstantBuffer s_constantBufferData;	//�R���X�^���g�o�b�t�@


///////////////////////////////////////////////////////////////////////////////////////////////////
CRendererDx12::CRendererDx12()
{
	m_factory = nullptr;
	m_adapter = nullptr;
	m_device = nullptr;
	m_cmdQue = nullptr;
	m_fenceHdl = HANDLE();
	m_queueFence = nullptr;
	m_swapChain = nullptr;
	m_commandList = nullptr;
	m_commandAllocator = nullptr;

	memset(m_renderTrg, 0, sizeof(m_renderTrg));
	m_descriptHeapRtv = nullptr;
	memset(m_descriptHdlRtv, 0, sizeof(m_descriptHdlRtv));

	m_depthBuffer = nullptr;
	m_descriptHeapDepth = nullptr;
	m_descriptHdlDepth = D3D12_CPU_DESCRIPTOR_HANDLE();

	m_pipelineState = nullptr;
	m_rootSignature = nullptr;

	m_frames = 0;
	m_rtvIndex = 0;

}

CRendererDx12::~CRendererDx12()
{
}

b8 CRendererDx12::Initialize(const CRendererDx12::INIT_PARAM& _param)
{
	m_hWnd = _param.hwnd;
	m_windowWidth = _param.width;
	m_windowHeight = _param.height;

	HRESULT hr;
	hr = __CreateFactory();
	if (FAILED(hr))
	{
		return false;
	}

	hr = __CreateDevice();
	if (FAILED(hr))
	{
		return false;
	}

	hr = __CreateCommandQueue();
	if (FAILED(hr))
	{
		return false;
	}

	hr = __CreateSwapChain();
	if (FAILED(hr))
	{
		return false;
	}

	hr = __CreateRenderTargetView();
	if (FAILED(hr))
	{
		return false;
	}

	hr = __CreateDepthStencilBuffer();
	if (FAILED(hr))
	{
		return false;
	}

	hr = __CreateCommandList();
	if (FAILED(hr))
	{
		return false;
	}

	//�R�}���h�L���[�p�̃t�F���X�̐���
	hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_queueFence.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	m_fenceHdl = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (m_fenceHdl == nullptr) 
	{
		return false;
	}

	m_viewPort.TopLeftX = 0.f;
	m_viewPort.TopLeftY = 0.f;
	m_viewPort.Width = SCast<f32>(m_windowWidth);
	m_viewPort.Height = SCast<f32>(m_windowHeight);
	m_viewPort.MinDepth = 0.f;
	m_viewPort.MaxDepth = 1.f;

	m_scissorRect.top = 0;
	m_scissorRect.left = 0;
	m_scissorRect.right = m_windowWidth;
	m_scissorRect.bottom = m_windowHeight;

	return true;
}

void CRendererDx12::Finalize()
{
	__WaitForPreviousFrame();

	// �C�x���g�n���h�������.
	CloseHandle(m_fenceHdl);
	m_fenceHdl = nullptr;

	for (UINT i = 0; i<RTV_NUM; ++i)
	{
		m_renderTrg[i].Reset();
	}

	m_depthBuffer.Reset();

	m_descriptHeapRtv.Reset();
	m_descriptHeapDepth.Reset();

	m_swapChain.Reset();
	m_queueFence.Reset();
	m_commandList.Reset();
	m_commandAllocator.Reset();
	m_adapter.Reset();
	m_cmdQue.Reset();
	m_device.Reset();
	m_factory.Reset();
}

// �`��J�n
void CRendererDx12::BeginDraw()
{
#if 1
	// ��]�p�𑝂₷.
	static f32 rot = 0.0f;
	rot = (0.01f / 0.5f);
	
	// ���[���h�s����X�V.
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

	LIB_KATA::MatrixUtil::Transform(ret[0], retMtx, LIB_KATA::CVector4::Left + LIB_KATA::CVector4(0.0f,0.0f, 0.0f, 1.0f));
	LIB_KATA::MatrixUtil::Transform(ret[1], retMtx, LIB_KATA::CVector4::Right + LIB_KATA::CVector4(0.0f, 0.0f, 0.0f, 1.0f));
	LIB_KATA::MatrixUtil::Transform(ret[2], retMtx, LIB_KATA::CVector4::Up + LIB_KATA::CVector4(0.0f, 0.0f, 0.0f, 1.0f));

	s_constantBufferData.World = retMtx;
#endif

	// �萔�o�b�t�@���X�V.
	memcpy(m_cbvDataBegin, &s_constantBufferData, sizeof(s_constantBufferData));

	// �R�}���h�A���P�[�^�ƃR�}���h���X�g�����Z�b�g.
	HRESULT hr = m_commandAllocator->Reset();
	if (FAILED(hr)){ return; }

	// �R�}���h���X�g�����Z�b�g�i���̎���Allocator�ƃp�C�v���C����ݒ�j
	hr = m_commandList->Reset(m_commandAllocator.Get(), nullptr);
	if (FAILED(hr)) { return; }

	//�r���[�|�[�g�ƃV�U�[��`�̐ݒ�
	m_commandList->RSSetViewports(1, &m_viewPort);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	//���\�[�X�̏�Ԃ��v���[���g�p���烌���_�[�^�[�Q�b�g�p�ɕύX
	__SetResourceBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//�����_�[�^�[�Q�b�g�̐ݒ�
	m_commandList->OMSetRenderTargets(1, &m_descriptHdlRtv[m_rtvIndex], FALSE, &m_descriptHdlDepth);

	//�[�x�o�b�t�@�ƃ����_�[�^�[�Q�b�g�̃N���A
	LIB_KATA::Color clearColor(0.0f, 0.5f, 0.0f, 1.0f);
	m_commandList->ClearRenderTargetView(m_descriptHdlRtv[m_rtvIndex], clearColor.color, 0, nullptr);
	m_commandList->ClearDepthStencilView(m_descriptHdlDepth, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ����Ńp�C�v���C����ݒ肷�邱�Ƃ��\����
	m_commandList->SetPipelineState(m_pipelineState.Get());

	//���[�g�V�O�l�`���̐ݒ�
	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

	// �f�B�X�N���v�^�q�[�v��ݒ�
	m_commandList->SetDescriptorHeaps(1, m_cbvHeap.GetAddressOf());

	// �f�B�X�N���v�^�q�[�v�e�[�u����ݒ�
	m_commandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
}

void CRendererDx12::DrawPolygon(const RENDER_DRAW_POLYGON_PARAM& _param)
{
	// �v���~�e�B�u�g�|���W�[�̐ݒ�.
	m_commandList->IASetPrimitiveTopology( SCast<D3D_PRIMITIVE_TOPOLOGY>(_param.drawType));

	// ���_�o�b�t�@�r���[��ݒ�.
	m_commandList->IASetVertexBuffers(0, 1, _param.vtxBuff->GetVertexView());

	// �`��R�}���h�𐶐�.
	m_commandList->DrawInstanced(_param.vtxBuff->GetVertexNum(), 1, 0, 0);
}

void CRendererDx12::EndDraw()
{
	HRESULT hr;

	//���\�[�X�̏�Ԃ������_�[�^�[�Q�b�g����v���[���g�p�ɕύX
	__SetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	hr = m_commandList->Close();

	// �R�}���hQue�ɂ�����l�ߍ��ݎ��s����
	ID3D12CommandList* command_lists[] = { m_commandList.Get() };
	m_cmdQue->ExecuteCommandLists(1, command_lists);

	hr = m_swapChain->Present(1, 0);
	if (FAILED(hr))
	{
		return;
	}

	//���s�����R�}���h�̏I���҂�
	__WaitForPreviousFrame();

	//�J�����g�̃o�b�N�o�b�t�@�̃C���f�b�N�X���擾����
	m_rtvIndex = m_swapChain->GetCurrentBackBufferIndex();
}

// �f�o�C�X�̎擾
CRendererDx12::DEVICE* CRendererDx12::GetDevice()
{
	return m_device.Get();
}

//========================================================================================================================

HRESULT CRendererDx12::__CreateFactory()
{
	HRESULT hr{};
	UINT flag{};

	//�f�o�b�O���[�h�̏ꍇ�̓f�o�b�O���C���[��L���ɂ���
#if defined(_DEBUG)
	{
		WinShPtr<ID3D12Debug> debug;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));
		if (FAILED(hr)) {
			return hr;
		}
		debug->EnableDebugLayer();
	}

	flag |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	//�t�@�N�g���̍쐬
	hr = CreateDXGIFactory2(flag, IID_PPV_ARGS(m_factory.GetAddressOf()));
	if (FAILED(hr)) {
		return hr;
	}
	return hr;
}

HRESULT CRendererDx12::__CreateDevice()
{
	HRESULT hr{};

	//�ŏ��Ɍ��������A�_�v�^�[���g�p����
	hr = m_factory->EnumAdapters(0, RCast<IDXGIAdapter**>(m_adapter.GetAddressOf()));
	if (SUCCEEDED(hr)) {

		//���������A�_�v�^���g�p���ăf�o�C�X���쐬����
		hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(m_device.GetAddressOf()));
	}

	return hr;
}

HRESULT CRendererDx12::__CreateCommandQueue()
{
	HRESULT hr{};
	D3D12_COMMAND_QUEUE_DESC command_queue_desc{};

	// �R�}���h�L���[�𐶐�.
	command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	//command_queue_desc.Priority = 0;
	command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	//command_queue_desc.NodeMask = 0;
	hr = m_device->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(m_cmdQue.GetAddressOf()));
	if (FAILED(hr)) {
		return hr;
	}
	return hr;
}

HRESULT CRendererDx12::__CreateSwapChain()
{
	HRESULT hr{};
	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
	swap_chain_desc.BufferDesc.Width = m_windowWidth;
	swap_chain_desc.BufferDesc.Height = m_windowHeight;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.OutputWindow = m_hWnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.BufferCount = 2;

	WinShPtr<IDXGISwapChain> swap_chain{};
	hr = m_factory->CreateSwapChain(m_cmdQue.Get(), &swap_chain_desc, swap_chain.GetAddressOf());
	if (FAILED(hr)) {
		return hr;
	}

	hr = swap_chain->QueryInterface(m_swapChain.GetAddressOf());
	if (FAILED(hr)) {
		return hr;
	}

	//�J�����g�̃o�b�N�o�b�t�@�̃C���f�b�N�X���擾����
	m_rtvIndex = m_swapChain->GetCurrentBackBufferIndex();

	return S_OK;
}

HRESULT CRendererDx12::__CreateRenderTargetView()
{
	HRESULT hr{};
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};

	//RTV�p�f�X�N���v�^�q�[�v�̍쐬
	heap_desc.NumDescriptors = RTV_NUM;
	heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	//heap_desc.NodeMask = 0;
	hr = m_device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(m_descriptHeapRtv.GetAddressOf()));
	if (FAILED(hr)) {
		return hr;
	}

	//�����_�[�^�[�Q�b�g���쐬���鏈��
	D3D12_RENDER_TARGET_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;
	desc.Texture2D.PlaneSlice = 0;

	u32 size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (u32 i = 0; i < RTV_NUM; ++i) {
		//�X���b�v�`�F�C������o�b�t�@���󂯎��
		hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_renderTrg[i].GetAddressOf()));
		if (FAILED(hr)) {
			return hr;
		}

		//RenderTargetView���쐬���ăq�[�v�f�X�N���v�^�ɓo�^
		m_descriptHdlRtv[i] = m_descriptHeapRtv->GetCPUDescriptorHandleForHeapStart();
		m_descriptHdlRtv[i].ptr += size * i;
		m_device->CreateRenderTargetView(m_renderTrg[i].Get(), &desc, m_descriptHdlRtv[i]);
	}

	return hr;
}

HRESULT CRendererDx12::__CreateDepthStencilBuffer()
{
	HRESULT hr;

	//�[�x�o�b�t�@�p�̃f�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc{};
	descriptor_heap_desc.NumDescriptors = 1;
	descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor_heap_desc.NodeMask = 0;
	hr = m_device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&m_descriptHeapDepth));
	if (FAILED(hr)) {
		return hr;
	}


	//�[�x�o�b�t�@�̍쐬
	D3D12_HEAP_PROPERTIES heap_properties{};
	heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_properties.CreationNodeMask = 1;
	heap_properties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resource_desc{};
	resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resource_desc.Alignment = 0;
	resource_desc.Width = m_windowWidth;
	resource_desc.Height = m_windowHeight;
	resource_desc.DepthOrArraySize = 1;
	resource_desc.MipLevels = 0;
	resource_desc.Format = DXGI_FORMAT_D32_FLOAT;
	resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resource_desc.SampleDesc.Count = 1;
	resource_desc.SampleDesc.Quality = 0;
	resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clear_value{};
	clear_value.Format = DXGI_FORMAT_D32_FLOAT;
	clear_value.DepthStencil.Depth = 1.0f;
	clear_value.DepthStencil.Stencil = 0;

	hr = m_device->CreateCommittedResource(&heap_properties, 
		D3D12_HEAP_FLAG_NONE, 
		&resource_desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clear_value, 
		IID_PPV_ARGS(m_depthBuffer.GetAddressOf()));
	if (FAILED(hr)) {
		return hr;
	}


	//�[�x�o�b�t�@�̃r���[�̍쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};

	dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
	dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

	m_descriptHdlDepth = m_descriptHeapDepth->GetCPUDescriptorHandleForHeapStart();

	m_device->CreateDepthStencilView(m_depthBuffer.Get(), &dsv_desc, m_descriptHdlDepth);

	return hr;
}

HRESULT CRendererDx12::__CreateCommandList()
{
	HRESULT hr;

	//�R�}���h�A���P�[�^�̍쐬
	hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocator.GetAddressOf()));
	if (FAILED(hr)) {
		return hr;
	}

	//�R�}���h�A���P�[�^�ƃo�C���h���ăR�}���h���X�g���쐬����
	hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(m_commandList.GetAddressOf()));
	if (FAILED(hr)) {
		return hr;
	}
	// �O�̂���Close���Ă���
	m_commandList->Close();

	return hr;
}

HRESULT CRendererDx12::CreateRootSignature() 
{
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
		return hr;
	}

	// ���[�g�V�O�l�`���[�̐���
	hr = m_device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf()));
	if (FAILED(hr))
	{
		PRINT_ERROR("CreateRootSignature Error");
		return hr;
	}

	return hr;
}

HRESULT CRendererDx12::CreatePipeline()
{
	WinShPtr<ID3DBlob> pVSBlob;
	WinShPtr<ID3DBlob> pPSBlob;

	// ���_�V�F�[�_�̃t�@�C���p�X������.
	// �R���p�C���ςݒ��_�V�F�[�_��ǂݍ���.
	HRESULT hr = D3DReadFileToBlob(L"D:\\program\\programTest\\DirectX12\\DirectX12\\bin\\Debug\\x64\\shader_vs.cso", pVSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : D3DReadFileToBlob() Failed.");
		return hr;
	}

	// �s�N�Z���V�F�[�_�̃t�@�C���p�X������.
	// �R���p�C���ς݃s�N�Z���V�F�[�_��ǂݍ���.
	hr = D3DReadFileToBlob(L"D:\\program\\programTest\\DirectX12\\DirectX12\\bin\\Debug\\x64\\shader_ps.cso", pPSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : D3DReadFileToBlob() Failed.");
		return hr;
	}

	// ���̓��C�A�E�g�̐ݒ�.
	D3D12_INPUT_ELEMENT_DESC inputElements[] = {
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "VTX_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

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
	desc.pRootSignature = m_rootSignature.Get();
	desc.VS = { RCast<UINT8*>(pVSBlob->GetBufferPointer()), pVSBlob->GetBufferSize() };
	desc.PS = { RCast<UINT8*>(pPSBlob->GetBufferPointer()), pPSBlob->GetBufferSize() };
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
	hr = m_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pipelineState.GetAddressOf()));
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : ID3D12Device::CreateGraphicsPipelineState() Failed.");
		return hr;
	}
	return hr;
}

// �萔�o�b�t�@�̐���
HRESULT CRendererDx12::CreateConstantBuff()
{
	// �萔�o�b�t�@�ւ̏������݃N���X�̐���
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	HRESULT hr = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_cbvHeap.GetAddressOf()));
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : ID3D12Device::CreateDescriptorHeap() Failed.");
		return false;
	}

	// �q�[�v�v���p�e�B�̐ݒ�.
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	// ���\�[�X�̐ݒ�.
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = sizeof(ResConstantBuffer);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// ���\�[�X�𐶐�.
	hr = m_device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_constantBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : ID3D12Device::CreateCommittedResource() Failed.");
		return false;
	}

	// �萔�o�b�t�@�r���[�̐ݒ�.
	D3D12_CONSTANT_BUFFER_VIEW_DESC bufferDesc = {};
	bufferDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
	bufferDesc.SizeInBytes = sizeof(ResConstantBuffer);

	// �萔�o�b�t�@��Desciptor��Heap�ɐݒ�
	m_device->CreateConstantBufferView(&bufferDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

	// �}�b�v����. �A�v���P�[�V�����I���܂� Unmap ���Ȃ�.
	// "Keeping things mapped for the lifetime of the resource is okay." �Ƃ̂��ƁB
	hr = m_constantBuffer->Map(0, nullptr, RCast<void**>(&m_cbvDataBegin));
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : ID3D12Resource::Map() Failed.");
		return false;
	}

	// �A�X�y�N�g��Z�o.
	f32 aspectRatio = SCast<f32>(m_windowWidth) / SCast<f32>(m_windowHeight);

#if 1
	// �萔�o�b�t�@�f�[�^�̐ݒ�.
	LIB_KATA::MatrixUtil::Identity(s_constantBufferData.World);

	LIB_KATA::CMatrix44 view;
	LIB_KATA::MatrixUtil::CreateLookAt(LIB_KATA::CVector3(0.0f, 0.0f, 5.0f), LIB_KATA::CVector3(0.0f, 0.0f, 0.0f), LIB_KATA::CVector3(0.0f, 1.0f, 0.0f), view);
	s_constantBufferData.View = view;

	LIB_KATA::CMatrix44 proj;
	LIB_KATA::MatrixUtil::CreatePerspectiveFieldOfView(LIB_KATA::PI_DIV4, aspectRatio, 1.0f, 1000.0f, proj);
	s_constantBufferData.Proj = proj;

#endif
	// �R�s�[����(�����ŃR�s�[�ł���A���C�����g��256�łȂ���NG)
	memcpy(m_cbvDataBegin, &s_constantBufferData, sizeof(s_constantBufferData));

	return hr;
}
void CRendererDx12::ReleaseConstantBuff()
{
	m_constantBuffer->Unmap(0, nullptr);

	m_constantBuffer.Reset();
	m_cbvHeap.Reset();

	m_cbvDataBegin = nullptr;
}

void CRendererDx12::ReleasePipeline()
{
	m_pipelineState.Reset();
}

void CRendererDx12::ReleaseRootSignature()
{
	m_rootSignature.Reset();
}

HRESULT CRendererDx12::__WaitForPreviousFrame()
{
	HRESULT hr;

	const UINT64 fence = m_frames;
	hr = m_cmdQue->Signal(m_queueFence.Get(), fence);
	if (FAILED(hr)) 
	{
		return -1;
	}
	++m_frames;

	// �V�O�i�����I��������҂�
	if (m_queueFence->GetCompletedValue() < fence) 
	{
		hr = m_queueFence->SetEventOnCompletion(fence, m_fenceHdl);
		if (FAILED(hr)) 
		{
			return -1;
		}

		WaitForSingleObject(m_fenceHdl, INFINITE);
	}
	return S_OK;
}

HRESULT CRendererDx12::__SetResourceBarrier(D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState)
{
	D3D12_RESOURCE_BARRIER resource_barrier{};

	resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resource_barrier.Transition.pResource = m_renderTrg[m_rtvIndex].Get();
	resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resource_barrier.Transition.StateBefore = BeforeState;
	resource_barrier.Transition.StateAfter = AfterState;

	m_commandList->ResourceBarrier(1, &resource_barrier);
	return S_OK;
}

