#include "shader\shader.h"

#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>

CShader::CShader()
{
}

CShader::~CShader()
{
}

b8 CShader::Create(const CShader::INIT_PARAM& _param)
{
	// シェーダのファイルパスを検索.
	// コンパイル済み頂点シェーダを読み込む.
	HRESULT hr = D3DReadFileToBlob(RCast<const w16* const>(_param.shaderPath), &m_shaderBuff);
	if (FAILED(hr))
	{
		PRINT_ERROR("Error : D3DReadFileToBlob() Failed.");
		return false;
	}
	return true;
}

void CShader::Release()
{
	if (m_shaderBuff)
	{
		m_shaderBuff->Release();
		m_shaderBuff = nullptr;
	}
}

void* CShader::GetBuffer() const
{
	return m_shaderBuff->GetBufferPointer();
}

size_t	CShader::GetBufferSize() const
{
	return m_shaderBuff->GetBufferSize();
}