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
GraphicsClass에 의해 ConstructFrustum 함수가 매 프레임마다 호출됩니다.
GraphicsClass에서는 화면의 depth와 투영 행렬, 그리고 뷰 행렬을 전달합니다.
이 입력들을 이용하여 현재 프레임의 시야 프러스텀 행렬을 만듭니다.
새로운 프러스텀 행렬로 시야 프러스텀을 구성하는 여섯 개의 평면을 계산해 냅니다.
*/
void Frustum::ConstructFrustum(float screenDepth, D3DXMATRIX projectionMatrix, D3DXMATRIX viewMatrix)
{
	float zMinimum, r;
	D3DXMATRIX matrix;

	//절두체의 최소 Z 거리를 계산한다.
	zMinimum = -projectionMatrix._43 / projectionMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix._33 = r;
	projectionMatrix._43 = -r * zMinimum;

	// 뷰 매트릭스와 업데이트된 투영 매트릭스에서 절두체 매트릭스를 생성하십시오.
	D3DXMatrixMultiply(&matrix, &viewMatrix, &projectionMatrix);

	//절두체 근처의 평면을 계산한다.
	m_Planes[0].a = matrix._14 + matrix._13;
	m_Planes[0].b = matrix._24 + matrix._23;
	m_Planes[0].c = matrix._34 + matrix._33;
	m_Planes[0].d = matrix._44 + matrix._43;
	D3DXPlaneNormalize(&m_Planes[0], &m_Planes[0]);

	// 절두체의 먼 거리의 평면을 계산한다.
	m_Planes[1].a = matrix._14 - matrix._13;
	m_Planes[1].b = matrix._24 - matrix._23;
	m_Planes[1].c = matrix._34 - matrix._33;
	m_Planes[1].d = matrix._44 - matrix._43;
	D3DXPlaneNormalize(&m_Planes[1], &m_Planes[1]);

	//절두체의 왼 평면을 계산한다.
	m_Planes[2].a = matrix._14 + matrix._11;
	m_Planes[2].b = matrix._24 + matrix._21;
	m_Planes[2].c = matrix._34 + matrix._31;
	m_Planes[2].d = matrix._44 + matrix._41;
	D3DXPlaneNormalize(&m_Planes[2], &m_Planes[2]);


	//절두체의 오른 평면을 계산한다.
	m_Planes[3].a = matrix._14 - matrix._11;
	m_Planes[3].b = matrix._24 - matrix._21;
	m_Planes[3].c = matrix._34 - matrix._31;
	m_Planes[3].d = matrix._44 - matrix._41;
	D3DXPlaneNormalize(&m_Planes[3], &m_Planes[3]);

	//절두체의 위의 평면을 계산한다.
	m_Planes[4].a = matrix._14 - matrix._12;
	m_Planes[4].b = matrix._24 - matrix._22;
	m_Planes[4].c = matrix._34 - matrix._32;
	m_Planes[4].d = matrix._44 - matrix._42;
	D3DXPlaneNormalize(&m_Planes[4], &m_Planes[4]);

	//절두체의 아래 평면을 계산한다.
	m_Planes[5].a = matrix._14 + matrix._12;
	m_Planes[5].b = matrix._24 + matrix._22;
	m_Planes[5].c = matrix._34 + matrix._32;
	m_Planes[5].d = matrix._44 + matrix._42;
	D3DXPlaneNormalize(&m_Planes[5], &m_Planes[5]);
}
/*
CheckPoint 함수는 하나의 점이 시야 프러스텀 내부에 있는지 확인합니다.
소개할 네 개의 체크 알고리즘들 중에서 가장 일반적이지만 올바른 환경에서 사용되면 또한 가장 효율적이기도 합니다.
이 함수는 점의 좌표를 입력으로 받고 모든 여섯 개의 평면 안에 점이 있는지를 확인합니다. 
리턴값은 포함 여부입니다.
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
CheckCube 함수는 정육면체의 8개의 꼭지점 중 어느 것이라도 시야 프러스텀 내에 있는지 확인합니다.
입력으로는 육면체의 중점과 반경만 있으면 되며 이를 이용하여 여덟개의 꼭지점 위치를 계산할 수 있습니다.
만약 꼭지점 하나라도 프러스텀 내부에 있다면 true를 리턴하고, 그렇지 않으면 false를 리턴합니다.
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
CheckSphere 함수는 구의 중심이 시야 프러스텀의 여섯 평면에 반경 이내만큼 존재하는지를 확인합니다.
어느 것이라도 바깥에 있으면 구체는 보이지 않는 것이고 함수는 false를 리턴할 것입니다.
모든 평면의 안쪽에 있다면 구체는 보이는 것이므로 true를 리턴합니다.
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
CheckRectangle 함수는 축마다 크기가 달라서 x반경, y반경, z반경을 각각 받는다는 것만
제외하면CheckCube함수와 작동방식이 동일합니다.
어쨌든 꼭지점 계산은 되므로 이 좌표를 구해 CheckCube와 동일하게 확인합니다.
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
