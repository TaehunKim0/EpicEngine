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

	//������ ����
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//ī�޶� ��ġ ����
	position = m_Position;

	//ī�޶� ���� ��
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//yaw(y��) pitch(x��) roll(z��) ȸ���� ���� ������ �����մϴ�.
	pitch = m_Rotation.x * 0.0174532925f;
	yaw = m_Rotation.y * 0.0174532925f;
	roll = m_Rotation.z * 0.0174532925f;

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	//lookAt �� up ���͸� ȸ�� ��ķ� �����Ͽ� �䰡 �������� �ùٸ��� ȸ���ǵ����մϴ�.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	//D3DXVec3TransformCoord : ���� ��ȯ�� ���� �Լ� , ���Ϳ� ����� ���ؼ� ����� ����� ������ �Ǵ� ���͸� ��ȯ

	//ȸ�� �� ī�޶� ��ġ�� ��� ��ġ�� ��ȯ�մϴ�.
	lookAt = position + lookAt;

	//���������� ������Ʈ �� �� ���Ϳ��� �� ����� ����ϴ�.
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);

}

void Camera::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}
