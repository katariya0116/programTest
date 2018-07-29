#pragma once

class CShader
{
public:
	struct INIT_PARAM
	{
		const c8* vsShaderName = nullptr;
		const c8* psShaderName = nullptr;

	};
public:
	CShader();
	virtual ~CShader();

public:
	b8 Initialize(const INIT_PARAM& _param);
	void Finalize();
};