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
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);

	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();
	D3DXVECTOR4 GetSpecularColor();
	float GetSpecularPower();

private:
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR4 m_ambientColor;
	D3DXVECTOR4 m_specularColor;
	float m_specularPower;

};

#endif
/*
�ֺ����� ����
�ֺ��� = �޺��� ���� �ʾƵ� �Ϻ� �ݻ�Ǵ� �������� ���� ��ġ�� �Ͱ��� ȿ��

�ȼ� ���̴����� �� ó�� �� �ȼ����� ���� ������ �ֺ����� �ش��ϴ� ���� �ݴϴ�.
�� �ڿ� �̷������ ������ ���� �� �ֺ��� ���� ���� ������ ���ϴ� ��.
*/