#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <d3dx10math.h>

/*
우리는 HLSL의 프로그래밍과, 정점/인덱스 버퍼를 셋업하는 방법 그리고 ColorShaderClass를 사용하여 HLSL 셰이더를 호출하는 방법을 살펴보았습니다.
하지만 우리가 한가지 놓친 것이 있는데, 그것은 바로 월드에서 우리가 보는 시점입니다.

이를 구현하기 위해서는 어떻게 우리가 장면을 보는지에 대한 정보를 DirectX 11에게 전달하는 카메라 클래스가 필요합니다.
카메라 클래스는 카메라의 위치와 현재 회전 상태를 계속 가지고 있어야 합니다.
또한 이 정보를 이용하여 렌더링시에 HLSL 셰이더에서 사용할 뷰 행렬을 생성합니다.
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
