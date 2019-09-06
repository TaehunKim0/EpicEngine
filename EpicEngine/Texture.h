#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3dx11.h>
#include <d3dx11tex.h>
class Texture
{
public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(ID3D11Device*, WCHAR*);
	void ShutDown();

	ID3D11ShaderResourceView* GetTexture();
private:
	ID3D11ShaderResourceView* m_Texture;
};

#endif

