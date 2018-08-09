#pragma once

#include "renderer/render.h"
#include "math/matrix.h"
#include "vertex_buff/vertex_buff.h"

#include <windows.h>

#include <d3d12.h>
#include <d3d12shader.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#include <wrl/client.h>

//===================================================================================
/// 描画ユーティリティ
//===================================================================================
struct INIT_RENDERER_DATA_DX12
{
	HWND hwnd;
	u32 width;
	u32 height;
	b8 windowMode;
};

//===================================================================================
/// 描画ユーティリティ
//===================================================================================
class CRendererDx12 : public TRenderer<ID3D12Device, INIT_RENDERER_DATA_DX12>
{
public:
	template<class TClass>
	using WinShPtr = Microsoft::WRL::ComPtr<TClass>;

public:
	static const u32 RTV_NUM = 2;

public:
	CRendererDx12();
	virtual ~CRendererDx12();

public:
	b8 Initialize(const INIT_PARAM& _param) override;
	void Finalize() override;

public:
	
	void BeginDraw() override;

	void EndDraw() override;

public:
	void DrawPolygon(const RENDER_DRAW_POLYGON_PARAM& _param) override;

public:
	DEVICE* GetDevice() override;

public:
	HRESULT CreateRootSignature();
	HRESULT CreatePipeline();
	HRESULT CreateConstantBuff();

public:
	void ReleaseConstantBuff();
	void ReleasePipeline();
	void ReleaseRootSignature();

private:
	HRESULT __CreateFactory();
	HRESULT __CreateDevice();
	HRESULT __CreateCommandQueue();
	HRESULT __CreateSwapChain();
	HRESULT __CreateRenderTargetView();
	HRESULT __CreateDepthStencilBuffer();
	HRESULT __CreateCommandList();

	HRESULT __WaitForPreviousFrame();
	HRESULT __SetResourceBarrier(D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState);
	HRESULT __PopulateCommandList();

private:
	HWND						m_hWnd;
	WinShPtr<IDXGIFactory4>		m_factory;
	WinShPtr<IDXGIAdapter3>		m_adapter;
	WinShPtr<ID3D12Device>		m_device;
	WinShPtr<ID3D12CommandQueue>	m_cmdQue;
	HANDLE							m_fenceHdl;
	WinShPtr<ID3D12Fence>			m_queueFence;
	WinShPtr<IDXGISwapChain3>		m_swapChain;

	WinShPtr<ID3D12GraphicsCommandList>	m_commandList;
	WinShPtr<ID3D12CommandAllocator>	m_commandAllocator;

	WinShPtr<ID3D12Resource>			m_renderTrg[RTV_NUM];
	WinShPtr<ID3D12DescriptorHeap>		m_descriptHeapRtv;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_descriptHdlRtv[RTV_NUM];

	WinShPtr<ID3D12Resource>			m_depthBuffer;
	WinShPtr<ID3D12DescriptorHeap>		m_descriptHeapDepth;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_descriptHdlDepth;

	// パイプライン情報
	WinShPtr<ID3D12PipelineState>		m_pipelineState;
	WinShPtr<ID3D12RootSignature>		m_rootSignature;
	WinShPtr<ID3D12Resource>             m_constantBuffer;		// 定数バッファ
	WinShPtr<ID3D12DescriptorHeap>       m_cbvHeap;				// 定数バッファ書き込みクラス
	u8*                                  m_cbvDataBegin;		// CBVの書き込みメモリ開始位置

	D3D12_RECT					m_scissorRect;
	D3D12_VIEWPORT				m_viewPort;

	s32							m_windowWidth;
	s32							m_windowHeight;

	u64 m_frames;
	u32 m_rtvIndex;
};
