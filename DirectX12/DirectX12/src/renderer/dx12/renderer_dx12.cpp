#include "renderer_dx12.h"
#include "vertex_buff\vertex_buff.h"
#include "shader/shader.h"
#include "math/vector.h"
#include "pipeline\pipeline.h"
#include <dxgidebug.h>



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

	//コマンドキュー用のフェンスの生成
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

	// イベントハンドルを閉じる.
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

// 描画開始
void CRendererDx12::BeginDraw()
{
	// コマンドアロケータとコマンドリストをリセット.
	HRESULT hr = m_commandAllocator->Reset();
	if (FAILED(hr)){ return; }

	// コマンドリストをリセット（この時にAllocatorとパイプラインを設定）
	hr = m_commandList->Reset(m_commandAllocator.Get(), nullptr);
	if (FAILED(hr)) { return; }

	//ビューポートとシザー矩形の設定
	m_commandList->RSSetViewports(1, &m_viewPort);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	//リソースの状態をプレゼント用からレンダーターゲット用に変更
	__SetResourceBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//レンダーターゲットの設定
	m_commandList->OMSetRenderTargets(1, &m_descriptHdlRtv[m_rtvIndex], FALSE, &m_descriptHdlDepth);

	//深度バッファとレンダーターゲットのクリア
	LIB_KATA::Color clearColor(0.0f, 0.5f, 0.0f, 1.0f);
	m_commandList->ClearRenderTargetView(m_descriptHdlRtv[m_rtvIndex], clearColor.color, 0, nullptr);
	m_commandList->ClearDepthStencilView(m_descriptHdlDepth, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

// 
void CRendererDx12::SetPipline(const CPipeline* _pipline)
{
	// これでパイプラインを設定することも可能だよ
	m_commandList->SetPipelineState(_pipline->GetPipelineState());

	//ルートシグネチャの設定
	m_commandList->SetGraphicsRootSignature(_pipline->GetRootSignature());

	// 定数バッファのディスクリプタヒープを設定
	ID3D12DescriptorHeap** heap = _pipline->GetConstBuffDHList();
	u32 num = _pipline->GetConstBuffNum();
	m_commandList->SetDescriptorHeaps(num, heap);
	for (u32 idx = 0; idx < num; ++idx)
	{
		m_commandList->SetGraphicsRootDescriptorTable(idx, heap[idx]->GetGPUDescriptorHandleForHeapStart());
	}
}

void CRendererDx12::DrawPolygon(const RENDER_DRAW_POLYGON_PARAM& _param)
{
	// プリミティブトポロジーの設定.
	m_commandList->IASetPrimitiveTopology( SCast<D3D_PRIMITIVE_TOPOLOGY>(_param.drawType));

	// 頂点バッファビューを設定.
	m_commandList->IASetVertexBuffers(0, 1, _param.vtxBuff->GetVertexView());

	// 描画コマンドを生成.
	m_commandList->DrawInstanced(_param.vtxBuff->GetVertexNum(), 1, 0, 0);
}

void CRendererDx12::EndDraw()
{
	HRESULT hr;

	//リソースの状態をレンダーターゲットからプレゼント用に変更
	__SetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	hr = m_commandList->Close();

	// コマンドQueにそれを詰め込み実行する
	ID3D12CommandList* command_lists[] = { m_commandList.Get() };
	m_cmdQue->ExecuteCommandLists(1, command_lists);

	hr = m_swapChain->Present(1, 0);
	if (FAILED(hr))
	{
		return;
	}

	//実行したコマンドの終了待ち
	__WaitForPreviousFrame();

	//カレントのバックバッファのインデックスを取得する
	m_rtvIndex = m_swapChain->GetCurrentBackBufferIndex();
}

// デバイスの取得
CRendererDx12::DEVICE* CRendererDx12::GetDevice()
{
	return m_device.Get();
}

//========================================================================================================================

HRESULT CRendererDx12::__CreateFactory()
{
	HRESULT hr{};
	UINT flag{};

	//デバッグモードの場合はデバッグレイヤーを有効にする
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

	//ファクトリの作成
	hr = CreateDXGIFactory2(flag, IID_PPV_ARGS(m_factory.GetAddressOf()));
	if (FAILED(hr)) {
		return hr;
	}
	return hr;
}

HRESULT CRendererDx12::__CreateDevice()
{
	HRESULT hr{};

	//最初に見つかったアダプターを使用する
	hr = m_factory->EnumAdapters(0, RCast<IDXGIAdapter**>(m_adapter.GetAddressOf()));
	if (SUCCEEDED(hr)) {

		//見つかったアダプタを使用してデバイスを作成する
		hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(m_device.GetAddressOf()));
	}

	return hr;
}

HRESULT CRendererDx12::__CreateCommandQueue()
{
	HRESULT hr{};
	D3D12_COMMAND_QUEUE_DESC command_queue_desc{};

	// コマンドキューを生成.
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

	//カレントのバックバッファのインデックスを取得する
	m_rtvIndex = m_swapChain->GetCurrentBackBufferIndex();

	return S_OK;
}

HRESULT CRendererDx12::__CreateRenderTargetView()
{
	HRESULT hr{};
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};

	//RTV用デスクリプタヒープの作成
	heap_desc.NumDescriptors = RTV_NUM;
	heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	//heap_desc.NodeMask = 0;
	hr = m_device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(m_descriptHeapRtv.GetAddressOf()));
	if (FAILED(hr)) {
		return hr;
	}

	//レンダーターゲットを作成する処理
	D3D12_RENDER_TARGET_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;
	desc.Texture2D.PlaneSlice = 0;

	u32 size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (u32 i = 0; i < RTV_NUM; ++i) {
		//スワップチェインからバッファを受け取る
		hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_renderTrg[i].GetAddressOf()));
		if (FAILED(hr)) {
			return hr;
		}

		//RenderTargetViewを作成してヒープデスクリプタに登録
		m_descriptHdlRtv[i] = m_descriptHeapRtv->GetCPUDescriptorHandleForHeapStart();
		m_descriptHdlRtv[i].ptr += size * i;
		m_device->CreateRenderTargetView(m_renderTrg[i].Get(), &desc, m_descriptHdlRtv[i]);
	}

	return hr;
}

HRESULT CRendererDx12::__CreateDepthStencilBuffer()
{
	HRESULT hr;

	//深度バッファ用のデスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc{};
	descriptor_heap_desc.NumDescriptors = 1;
	descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor_heap_desc.NodeMask = 0;
	hr = m_device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&m_descriptHeapDepth));
	if (FAILED(hr)) {
		return hr;
	}


	//深度バッファの作成
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


	//深度バッファのビューの作成
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

	//コマンドアロケータの作成
	hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocator.GetAddressOf()));
	if (FAILED(hr)) {
		return hr;
	}

	//コマンドアロケータとバインドしてコマンドリストを作成する
	hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(m_commandList.GetAddressOf()));
	if (FAILED(hr)) {
		return hr;
	}
	// 念のためCloseしておく
	m_commandList->Close();

	return hr;
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

	// シグナルが終わったか待ち
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

