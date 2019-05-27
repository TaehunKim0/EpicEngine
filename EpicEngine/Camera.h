#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <d3dx10math.h>

/*
�츮�� HLSL�� ���α׷��ְ�, ����/�ε��� ���۸� �¾��ϴ� ��� �׸��� ColorShaderClass�� ����Ͽ� HLSL ���̴��� ȣ���ϴ� ����� ���캸�ҽ��ϴ�.
������ �츮�� �Ѱ��� ��ģ ���� �ִµ�, �װ��� �ٷ� ���忡�� �츮�� ���� �����Դϴ�.

�̸� �����ϱ� ���ؼ��� ��� �츮�� ����� �������� ���� ������ DirectX 11���� �����ϴ� ī�޶� Ŭ������ �ʿ��մϴ�.
ī�޶� Ŭ������ ī�޶��� ��ġ�� ���� ȸ�� ���¸� ��� ������ �־�� �մϴ�.
���� �� ������ �̿��Ͽ� �������ÿ� HLSL ���̴����� ����� �� ����� �����մϴ�.
*/

class Camera
{
public:
	Camera();
	Camera(const Camera& other);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

private:
	D3DXVECTOR3 m_Position;
	D3DXVECTOR3 m_Rotation;
	D3DXMATRIX m_viewMatrix;
};
#endif
