#pragma once
#include <math.h>

class Position
{
public:
	Position();
	Position(const Position&);
	~Position();

	void SetFrameTime(float);
	void GetRotation(float&);

	void TurnLeft(bool);
	void TurnRight(bool);

private:
	float m_FrameTime;
	float m_RotationY;
	float m_LeftTurnSpeed, m_RightTurnSpeed;
};

