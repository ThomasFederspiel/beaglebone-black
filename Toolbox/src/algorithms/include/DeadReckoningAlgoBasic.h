/*
 * DeadReckoningAlgoBasic.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef DEADRECKONINGALGOBASIC_H_
#define DEADRECKONINGALGOBASIC_H_

// standard
#include <math.h>

// local
#include "IDeadReckoningAlgo.h"
#include "DeadReckoningPose.h"

// project

class DeadReckoningAlgoBasic final : public IDeadReckoningAlgo
{
public:

	DeadReckoningAlgoBasic(const float wheelDistance_m) : IDeadReckoningAlgo(),
		m_wheelDistance_m(wheelDistance_m), m_pose()
	{
	}

	const DeadReckoningPose& getPose() const override
	{
		return m_pose;
	}

	void update(const float leftOdoDelta, const float rightOdoDelta) override
	{
		const float midOdoDelta = (leftOdoDelta + rightOdoDelta) / 2.f;

		const float theta = (rightOdoDelta - leftOdoDelta) / (2.0f * m_wheelDistance_m) + m_pose.getTheta();

		const float x = midOdoDelta * cos(theta) + m_pose.getX();

		const float y = midOdoDelta * sin(theta) + m_pose.getY();

		m_pose.set(x, y, theta);
	}

private:
	// in [m]
	float m_wheelDistance_m;

	DeadReckoningPose m_pose;
};

#endif /* DEADRECKONINGALGOBASIC_H_ */
