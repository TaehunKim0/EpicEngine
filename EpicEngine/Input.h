#pragma once
class Input
{
private:
	bool m_Keys[256];

public:
	Input();
	Input(const Input& other);
	~Input();

	void Initialize();

	void KeyDown(unsigned int input);
	void KeyUp(unsigned int input);

	bool IsKeyDown(unsigned int key);
};

