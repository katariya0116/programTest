#include "vertex_buff/vertex_buff.h"
#include "renderer/dx12/renderer_dx12.h"
#include "renderer/renderer_utility.h"

CVertexBuff::CVertexBuff()
{
	m_vertexBuffer = nullptr;
	m_vertexBufferView.BufferLocation = 0;
	m_vertexBufferView.SizeInBytes = 0;
	m_vertexBufferView.StrideInBytes = 0;
}

CVertexBuff::~CVertexBuff()
{
}

b8 CVertexBuff::Create(const CREATE_VTX_BUFF_PARAM& _param)
{
	m_vertexNum = _param.vertexNum;

	CRendererDx12* render = SCast<CRendererDx12*>(CRenderUtility::GetRenderer());

	// ヒーププロパティの設定.
	D3D12_HEAP_PROPERTIES prop;
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	// リソースの設定.
	D3D12_RESOURCE_DESC desc;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = _param.size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// リソースを生成.
	HRESULT hr = render->GetDevice()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertexBuffer));
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : ID3D12Device::CreateCommittedResource() Failed.");
		return hr;
	}

	// GPU側にマップする.
	u8* pData;
	hr = m_vertexBuffer->Map(0, nullptr, RCast<void**>(&pData));
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : ID3D12Resource::Map() Failed.");
		return false;
	}

	// 頂点データをコピー.
	memcpy(pData, _param._buff, _param.size);

	// アンマップする.
	m_vertexBuffer->Unmap(0, nullptr);

	// 頂点バッファビューの設定.
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = _param.stride;
	m_vertexBufferView.SizeInBytes = _param.size;

	return SUCCEEDED(hr);
}

void CVertexBuff::Release()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	// ロケーションを初期化
	m_vertexBufferView.BufferLocation = 0;
	m_vertexBufferView.SizeInBytes = 0;
	m_vertexBufferView.StrideInBytes = 0;
}


//===================================================================================
//
CConstantBuff::CConstantBuff()
	: m_constantBuffer(nullptr)
	, m_cbvHeap(nullptr)
	, m_cbvDataBegin(nullptr)
{
}

CConstantBuff::~CConstantBuff()
{
}

b8 CConstantBuff::Create(const CREATE_PARAM& _param)
{
	CRendererDx12* render = SCast<CRendererDx12*>(CRenderUtility::GetRenderer());
	CRendererDx12::DEVICE* device = render->GetDevice();

	// 定数バッファへの書き込みクラスの生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	HRESULT hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbvHeap));
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : ID3D12Device::CreateDescriptorHeap() Failed.");
		return false;
	}

	// ヒーププロパティの設定.
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	// リソースの設定.
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = _param.buffSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// リソースを生成.
	hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_constantBuffer));
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : ID3D12Device::CreateCommittedResource() Failed.");
		return false;
	}

	// 定数バッファビューの設定.
	D3D12_CONSTANT_BUFFER_VIEW_DESC bufferDesc = {};
	bufferDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
	bufferDesc.SizeInBytes = _param.buffSize;

	// 定数バッファのDesciptorをHeapに設定
	device->CreateConstantBufferView(&bufferDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

	// "Keeping things mapped for the lifetime of the resource is okay." とのこと。
	hr = m_constantBuffer->Map(0, nullptr, RCast<void**>(&m_cbvDataBegin));
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : ID3D12Resource::Map() Failed.");
		return false;
	}

	return true;
}

void CConstantBuff::Release()
{
	m_constantBuffer->Unmap(0, nullptr);

	if (m_constantBuffer)
	{
		m_constantBuffer->Release();
		m_constantBuffer = nullptr;
	}

	if (m_cbvHeap)
	{
		m_cbvHeap->Release();
		m_cbvHeap = nullptr;
	}

	m_cbvDataBegin = nullptr;
}

// バッファコピー
void CConstantBuff::CopyConstantBuff(const void* _buff, size_t buffSize)
{
	Assert((RCast<u64>(_buff) & 0x00000000000000ff) == 0);
	// コピーする(ここでコピーできるアライメントが256でないとNG)
	memcpy(m_cbvDataBegin, _buff, buffSize);

}

ID3D12DescriptorHeap* CConstantBuff::GetDescriptorHeap()
{
	return m_cbvHeap;
}
