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
SetFrameTime �Լ��� �� Ŭ������ ������ �ӵ��� �����ϱ� ���� ���˴ϴ�.
PositionClass�� �� ������ �ð��� ����Ͽ� ī�޶� �󸶳� ������ �����̰� ȸ���ϴ��� ����մϴ�.
�� �Լ��� ī�޶� �����̱� ���� �� �������� ���ۿ� �Ҹ����� �ؾ� �մϴ�.
*/
void Position::SetFrameTime(float time)
{
	m_FrameTime = time;
}

/*
GetRotation �Լ��� ī�޶��� Y�� ȸ�� ������ �����մϴ�.
�� Ʃ�丮���� ���� ���� ����� �Լ��� �������� ���߿� �� ���� ������ �˷��ֵ��� Ȯ��� �� �ֽ��ϴ�.
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
