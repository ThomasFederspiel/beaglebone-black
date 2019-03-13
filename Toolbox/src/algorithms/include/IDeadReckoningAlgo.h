/*
 * IDeadReckoningAlgo.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef IDEADRECKONINGALGO_H_
#define IDEADRECKONINGALGO_H_

// standard

// local

// project

class DeadReckoningPose;

class IDeadReckoningAlgo
{
public:

	virtual ~IDeadReckoningAlgo()
	{
	}

	virtual const DeadReckoningPose& getPose() const = 0;

	virtual void update(const float leftOdoDelta, const float rightOdoDelta) = 0;
};

#endif /* IDEADRECKONINGALGO_H_ */
