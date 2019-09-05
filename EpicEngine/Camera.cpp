#include"stdafx.h"
#include "Camera.h"


Camera::Camera()
{
	m_Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

Camera::Camera(const Camera& other)
{

}

Camera::~Camera()
{
}

void Camera::SetPosition(float x , float y , float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_Rotation.x = x;
	m_Rotation.y = y;
	m_Rotation.z = z;
}

D3DXVECTOR3 Camera::GetPosition()
{
	return m_Position;
}

D3DXVECTOR3 Camera::GetRotation()
{
	return m_Rotation;
}

void Camera::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	//업벡터 설정
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//카메라 위치 설정
	position = m_Position;

	//카메라가 보는 점
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//yaw(y축) pitch(x축) roll(z축) 회전을 라디안 단위로 설정합니다.
	pitch = m_Rotation.x * 0.0174532925f;
	yaw = m_Rotation.y * 0.0174532925f;
	roll = m_Rotation.z * 0.0174532925f;

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	//lookAt 및 up 벡터를 회전 행렬로 변형하여 뷰가 원점에서 올바르게 회전되도록합니다.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	//D3DXVec3TransformCoord : 벡터 변환을 위한 함수 , 벡터에 행렬을 곱해서 행렬이 적용된 포인터 또는 벡터를 반환

	//회전 된 카메라 위치를 뷰어 위치로 변환합니다.
	lookAt = position + lookAt;

	//마지막으로 업데이트 된 세 벡터에서 뷰 행렬을 만듭니다.
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);

}

void Camera::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}
