#pragma once

class IRenderer;

enum class RENDERER_TYPE
{
	NONE = 0,
	DX12,
	MAX,
};

/// �����_���[������
struct INIT_RENDER_PARAM
{
	RENDERER_TYPE type = RENDERER_TYPE::NONE;
	u32 width = 0;
	u32 height = 0;
	b8 windowMode = false;
};

//===================================================================================
/// �`�惆�[�e�B���e�B
//===================================================================================
class CRenderUtility
{
public:
	static b8 Initialize(const INIT_RENDER_PARAM& _param);

	static void Finalize();

public:
	static void Begin();

	static void End();

public:
	static IRenderer* GetRenderer();
};
