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

	//�켱 �𵨵��� ���ڸ� �����ϰ� ModelInfoType Ÿ���� �迭�� ����ϴ�.
	m_ModelCount = numModels;

	// �� ������ ��� �迭�� �����Ͻʽÿ�
	m_ModelInfoList = new ModelInfoType[m_ModelCount];
	if (!m_ModelInfoList)
	{
		return false;
	}

	//�������� �������� seed�� ���� �ð����� �ְ� ���Ƿ� ����� ��ġ�� ���Ͽ� �� �迭�� ����
	srand((unsigned int)time(NULL));

	//��� ���� ���캸�� �� ����� ��ġ�� �������� �����Ͻʽÿ�.
	for (i = 0; i < m_ModelCount; i++)
	{
		//������ ���� ���� ������ �����Ͻʽÿ�.
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_ModelInfoList[i].color = D3DXVECTOR4(red, green, blue, 1.0f);

		//��带 ���� ��� �տ� ������ ��ġ�� �����Ͻʽÿ�.
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
