#include "stdafx.h"
#include "Texture.h"


Texture::Texture()
{
	m_Texture = nullptr;
}

Texture::Texture(const Texture&) : Texture()
{
}

Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device* device, WCHAR* fileName)
{
	HRESULT result;

	result = D3DX11CreateShaderResourceViewFromFile(device, fileName, NULL, NULL, &m_Texture, NULL);
	if (FAILED(result))
		return false;

	return true;
}

void Texture::ShutDown()
{
	if (m_Texture)
	{
		m_Texture->Release();
		m_Texture = nullptr;
	}
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_Texture;
}
