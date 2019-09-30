#pragma once

#include <d3dx10math.h>
#include <stdlib.h>
#include <time.h>

class ModelList
{
private:
	struct ModelInfoType
	{
		D3DXVECTOR4 color;
		float positionX, positionY, positionZ;
	};

public:
	ModelList();
	ModelList(const ModelList&);
	~ModelList();

	bool Initialize(int);
	void Shutdown();

	int GetModelCount();
	void GetData(int, float&, float&, float&, D3DXVECTOR4&);

private:
	int m_ModelCount;
	ModelInfoType* m_ModelInfoList;
};

