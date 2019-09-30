#include "Frustum.h"



Frustum::Frustum()
{
}

Frustum::Frustum(const Frustum & other)
{
}

Frustum::~Frustum()
{
}
/*
GraphicsClass�� ���� ConstructFrustum �Լ��� �� �����Ӹ��� ȣ��˴ϴ�.
GraphicsClass������ ȭ���� depth�� ���� ���, �׸��� �� ����� �����մϴ�.
�� �Էµ��� �̿��Ͽ� ���� �������� �þ� �������� ����� ����ϴ�.
���ο� �������� ��ķ� �þ� ���������� �����ϴ� ���� ���� ����� ����� ���ϴ�.
*/
void Frustum::ConstructFrustum(float screenDepth, D3DXMATRIX projectionMatrix, D3DXMATRIX viewMatrix)
{
	float zMinimum, r;
	D3DXMATRIX matrix;

	//����ü�� �ּ� Z �Ÿ��� ����Ѵ�.
	zMinimum = -projectionMatrix._43 / projectionMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix._33 = r;
	projectionMatrix._43 = -r * zMinimum;

	// �� ��Ʈ������ ������Ʈ�� ���� ��Ʈ�������� ����ü ��Ʈ������ �����Ͻʽÿ�.
	D3DXMatrixMultiply(&matrix, &viewMatrix, &projectionMatrix);

	//����ü ��ó�� ����� ����Ѵ�.
	m_Planes[0].a = matrix._14 + matrix._13;
	m_Planes[0].b = matrix._24 + matrix._23;
	m_Planes[0].c = matrix._34 + matrix._33;
	m_Planes[0].d = matrix._44 + matrix._43;
	D3DXPlaneNormalize(&m_Planes[0], &m_Planes[0]);

	// ����ü�� �� �Ÿ��� ����� ����Ѵ�.
	m_Planes[1].a = matrix._14 - matrix._13;
	m_Planes[1].b = matrix._24 - matrix._23;
	m_Planes[1].c = matrix._34 - matrix._33;
	m_Planes[1].d = matrix._44 - matrix._43;
	D3DXPlaneNormalize(&m_Planes[1], &m_Planes[1]);

	//����ü�� �� ����� ����Ѵ�.
	m_Planes[2].a = matrix._14 + matrix._11;
	m_Planes[2].b = matrix._24 + matrix._21;
	m_Planes[2].c = matrix._34 + matrix._31;
	m_Planes[2].d = matrix._44 + matrix._41;
	D3DXPlaneNormalize(&m_Planes[2], &m_Planes[2]);


	//����ü�� ���� ����� ����Ѵ�.
	m_Planes[3].a = matrix._14 - matrix._11;
	m_Planes[3].b = matrix._24 - matrix._21;
	m_Planes[3].c = matrix._34 - matrix._31;
	m_Planes[3].d = matrix._44 - matrix._41;
	D3DXPlaneNormalize(&m_Planes[3], &m_Planes[3]);

	//����ü�� ���� ����� ����Ѵ�.
	m_Planes[4].a = matrix._14 - matrix._12;
	m_Planes[4].b = matrix._24 - matrix._22;
	m_Planes[4].c = matrix._34 - matrix._32;
	m_Planes[4].d = matrix._44 - matrix._42;
	D3DXPlaneNormalize(&m_Planes[4], &m_Planes[4]);

	//����ü�� �Ʒ� ����� ����Ѵ�.
	m_Planes[5].a = matrix._14 + matrix._12;
	m_Planes[5].b = matrix._24 + matrix._22;
	m_Planes[5].c = matrix._34 + matrix._32;
	m_Planes[5].d = matrix._44 + matrix._42;
	D3DXPlaneNormalize(&m_Planes[5], &m_Planes[5]);
}
/*
CheckPoint �Լ��� �ϳ��� ���� �þ� �������� ���ο� �ִ��� Ȯ���մϴ�.
�Ұ��� �� ���� üũ �˰���� �߿��� ���� �Ϲ��������� �ùٸ� ȯ�濡�� ���Ǹ� ���� ���� ȿ�����̱⵵ �մϴ�.
�� �Լ��� ���� ��ǥ�� �Է����� �ް� ��� ���� ���� ��� �ȿ� ���� �ִ����� Ȯ���մϴ�. 
���ϰ��� ���� �����Դϴ�.
*/
bool Frustum::CheckPoint(float x , float y , float z)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(x, y, z)) < 0.0f)
		{
			return false;
		}
	}

	return true;
}
/*
CheckCube �Լ��� ������ü�� 8���� ������ �� ��� ���̶� �þ� �������� ���� �ִ��� Ȯ���մϴ�.
�Է����δ� ����ü�� ������ �ݰ游 ������ �Ǹ� �̸� �̿��Ͽ� �������� ������ ��ġ�� ����� �� �ֽ��ϴ�.
���� ������ �ϳ��� �������� ���ο� �ִٸ� true�� �����ϰ�, �׷��� ������ false�� �����մϴ�.
*/
bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

/*
CheckSphere �Լ��� ���� �߽��� �þ� ���������� ���� ��鿡 �ݰ� �̳���ŭ �����ϴ����� Ȯ���մϴ�.
��� ���̶� �ٱ��� ������ ��ü�� ������ �ʴ� ���̰� �Լ��� false�� ������ ���Դϴ�.
��� ����� ���ʿ� �ִٸ� ��ü�� ���̴� ���̹Ƿ� true�� �����մϴ�.
*/

bool Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(xCenter, yCenter, zCenter)) < -radius)
		{
			return false;
		}
	}

	return true;
}

/*
CheckRectangle �Լ��� �ึ�� ũ�Ⱑ �޶� x�ݰ�, y�ݰ�, z�ݰ��� ���� �޴´ٴ� �͸�
�����ϸ�CheckCube�Լ��� �۵������ �����մϴ�.
��·�� ������ ����� �ǹǷ� �� ��ǥ�� ���� CheckCube�� �����ϰ� Ȯ���մϴ�.
*/
bool Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}
