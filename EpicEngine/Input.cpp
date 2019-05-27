#include "stdafx.h"
#include "Input.h"

Input::Input()
{
	ZeroMemory(&m_Keys, sizeof(m_Keys));
}

Input::Input(const Input& other) : Input()
{
}

Input::~Input()
{
}

void Input::Initialize()
{
	for (int i = 0; i < 256; i++)
		m_Keys[i] = false;
}

void Input::KeyDown(unsigned int input)
{
	m_Keys[input] = true;
}

void Input::KeyUp(unsigned int input)
{
	m_Keys[input] = false;
}

bool Input::IsKeyDown(unsigned int key)
{
	return m_Keys[key];
}
