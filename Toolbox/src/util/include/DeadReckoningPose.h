/*
 * DeadReckoningPose.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef DEADRECKONINGPOSE_H_
#define DEADRECKONINGPOSE_H_

// standard

// local

// project

class DeadReckoningPose
{
public:

	DeadReckoningPose() : m_x(0.f), m_y(0.f), m_theta(0.f)
	{
	}

	void set(const float x, const float y, const float theta)
	{
		m_x = x;
		m_y = y;
		m_theta = theta;
	}

	void setX(const float x)
	{
		m_x = x;
	}

	void setY(const float y)
	{
		m_y = y;
	}

	void setTheta(const float theta)
	{
		m_theta = theta;
	}

	float getX() const
	{
		return m_x;
	}

	float getY() const
	{
		return m_y;
	}

	float getTheta() const
	{
		return m_theta;
	}

private:
	float m_x;
	float m_y;
	float m_theta;
};

#endif /* DEADRECKONINGPOSE_H_ */
