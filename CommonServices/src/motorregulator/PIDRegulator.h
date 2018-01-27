/*
 * PIDRegulator.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef PIDREGULATOR_H_
#define PIDREGULATOR_H_

// standard

// local

// project

class IMotor;

class PIDRegulator final
{
public:
	explicit PIDRegulator();

	bool isEnabled() const;
	void update();

private:
};

#endif /* PIDREGULATOR_H_ */
