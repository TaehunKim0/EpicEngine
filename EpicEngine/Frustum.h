#pragma once

#include <d3dX10math.h>

class Frustum
{
public:
	Frustum();
	Frustum(const Frustum&);
	~Frustum();

public:
	void ConstructFrustum(float, D3DXMATRIX, D3DXMATRIX);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);

private:
	D3DXPLANE m_Planes[6];
};

