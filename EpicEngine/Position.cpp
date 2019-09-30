#include "Position.h"



Position::Position()
{
	m_FrameTime = 0.f;
	m_LeftTurnSpeed = 0.f;
	m_RightTurnSpeed = 0.f;
	m_RotationY = 0.f;
}

Position::Position(const Position & other)
{
}


Position::~Position()
{
}

/*
SetFrameTime 함수는 이 클래스의 프레임 속도를 설정하기 위해 사용됩니다.
PositionClass는 이 프레임 시간을 사용하여 카메라가 얼마나 빠르게 움직이고 회전하는지 계산합니다.
이 함수는 카메라가 움직이기 전에 매 프레임의 시작에 불리도록 해야 합니다.
*/
void Position::SetFrameTime(float time)
{
	m_FrameTime = time;
}

/*
GetRotation 함수는 카메라의 Y축 회전 정도를 리턴합니다.
이 튜토리얼을 위해 만든 도우미 함수일 뿐이지만 나중에 더 많은 정보를 알려주도록 확장될 수 있습니다.
*/
void Position::GetRotation(float & y)
{
	y = m_RotationY;
}

void Position::TurnLeft(bool keydown)
{
	if (keydown)
	{
		m_LeftTurnSpeed += m_FrameTime * 0.01f;

		if (m_LeftTurnSpeed > (m_FrameTime * 0.15f))
		{
			m_LeftTurnSpeed = m_FrameTime * 0.15f;
		}
	}
	else
	{
		m_LeftTurnSpeed -= m_FrameTime * 0.005f;

		if (m_LeftTurnSpeed < 0.0f)
		{
			m_LeftTurnSpeed = 0.f;
		}
	}

	m_RotationY -= m_LeftTurnSpeed;
	if (m_RotationY < 0.0f)
	{
		m_RotationY += 360.0f;
	}

}

void Position::TurnRight(bool keydown)
{
	if (keydown)
	{
		m_RightTurnSpeed += m_FrameTime * 0.01f;

		if (m_RightTurnSpeed > (m_FrameTime * 0.15f))
		{
			m_RightTurnSpeed = m_FrameTime * 0.15f;
		}
	}
	else
	{
		m_RightTurnSpeed -= m_FrameTime * 0.005f;

		if (m_RightTurnSpeed < 0.0f)
		{
			m_RightTurnSpeed = 0.f;
		}
	}

	m_RotationY -= m_RightTurnSpeed;
	if (m_RotationY > 360.f)
	{
		m_RotationY -= 360.0f;
	}
}
