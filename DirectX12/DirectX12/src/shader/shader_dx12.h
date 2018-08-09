#pragma once

class CShaderDx12
{
public:
	struct INIT_PARAM
	{
		const c8* vsShaderName = nullptr;
		const c8* psShaderName = nullptr;

	};
public:
	CShaderDx12();
	virtual ~CShaderDx12();

public:
	b8 Initialize(const INIT_PARAM& _param);
	void Finalize();
};