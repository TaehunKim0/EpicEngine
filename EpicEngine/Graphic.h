#pragma once
#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include "D3D.h"
#include "Camera.h"
#include "Model.h"
#include "LightShader.h"
#include "Light.h"
#include "Bitmap.h"
#include "TextureShader.h"
#include "Text.h"

/*D3D Ŭ������ �ʱ�ȭ�ϰ� �����ϴ� �ڵ�*/
//BeginScene EndScene �� ȣ���� Direct3D�� ����Ͽ� �׸��� �κ�

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
	//bool Frame(int , int);

public:
	bool Frame(int , int ,int, int);
	bool Render();

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	//ColorShader* m_ColorShader;
	LightShader* m_LightShader;
	TextureShader* m_TextureShader;
	Light* m_Light;
	Bitmap* m_Bitmap;
	Text* m_Text;

	float translationOffset[3];
};

#endif // !_GRAPHIC_H_