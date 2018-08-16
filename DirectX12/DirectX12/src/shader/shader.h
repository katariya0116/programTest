#pragma once

struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;

enum class SHADER_TYPE
{
	NONE = 0,
	VS,
	PS,
	DS,
	HS,
	GS,
};

class CShader
{
public:
	struct INIT_PARAM
	{
		const w16*	shaderPath = nullptr;
		SHADER_TYPE	shaderType = SHADER_TYPE::NONE;
	};

public:
	CShader();
	~CShader();

public:
	b8 Create(const INIT_PARAM& _param);
	void Release();

public:
	void*	GetBuffer() const;
	size_t	GetBufferSize() const;

private:
	ID3DBlob*		m_shaderBuff;
	SHADER_TYPE		m_shaderType;
};