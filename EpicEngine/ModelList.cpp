#include "ModelList.h"



ModelList::ModelList()
{
	m_ModelInfoList = 0;
}

ModelList::ModelList(const ModelList & other)
{
}


ModelList::~ModelList()
{
}

bool ModelList::Initialize(int numModels)
{
	int i;
	float red, green, blue;

	//우선 모델들의 숫자를 저장하고 ModelInfoType 타입의 배열을 만듭니다.
	m_ModelCount = numModels;

	// 모델 정보의 목록 배열을 생성하십시오
	m_ModelInfoList = new ModelInfoType[m_ModelCount];
	if (!m_ModelInfoList)
	{
		return false;
	}

	//랜덤숫자 생성기의 seed를 현재 시간으로 주고 임의로 색상과 위치를 정하여 모델 배열에 저장
	srand((unsigned int)time(NULL));

	//모든 모델을 살펴보고 모델 색상과 위치를 무작위로 생성하십시오.
	for (i = 0; i < m_ModelCount; i++)
	{
		//모형에 대한 랜덤 색상을 생성하십시오.
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_ModelInfoList[i].color = D3DXVECTOR4(red, green, blue, 1.0f);

		//모드를 위해 뷰어 앞에 임의의 위치를 생성하십시오.
		m_ModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}

void ModelList::Shutdown()
{
	if (m_ModelInfoList)
	{
		delete[] m_ModelInfoList;
		m_ModelInfoList = 0;
	}
}

int ModelList::GetModelCount()
{
	return m_ModelCount;
}

void ModelList::GetData(int index, float & positionX, float & positionY, float & positionZ, D3DXVECTOR4 & color)
{
	positionX = m_ModelInfoList[index].positionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;

	color = m_ModelInfoList[index].color;
}
