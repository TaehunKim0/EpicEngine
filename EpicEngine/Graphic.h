#pragma once
#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include "D3D.h"
#include "Camera.h"
#include "Model.h"
#include "TextureShader.h"

/*D3D 클래스를 초기화하고 정리하는 코드*/
//BeginScene EndScene 를 호출해 Direct3D를 사용하여 그리는 부분

//Global
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3D;
class Graphic
{
public:
	Graphic();
	Graphic(const Graphic& other);
	~Graphic();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	//ColorShader* m_ColorShader;
	TextureShader* m_TextureShader;
};

#endif // !_GRAPHIC_H_