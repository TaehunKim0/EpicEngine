#pragma once
#ifndef _LIGHTCLASS_H_ 
#define _LIGHTCLASS_H_

#include <D3DX10math.h>

class Light
{
public:
	Light();
	Light(const Light&);
	~Light();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);

	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();

private:
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR4 m_ambientColor;
};

#endif
/*
주변광의 원리
주변광 = 햇빛이 닿지 않아도 일부 반사되는 빛때문에 빛이 비치는 것같은 효과

픽셸 셰이더에서 맨 처음 각 픽셀마다 일정 수준의 주변광에 해당하는 값을 줍니다.
그 뒤에 이루어지는 연산은 단지 그 주변광 값에 계산된 값들을 더하는 것.
*/