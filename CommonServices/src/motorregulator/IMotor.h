/*
 * IMotor.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef IMOTOR_H_
#define IMOTOR_H_

// standard
#include <cstdint>

// local

// project

class IMotor
{
public:
	virtual void coast() = 0;
	virtual void stop() = 0;
	virtual void setControlSignal(const bool forward, const uint16_t signal) = 0;
	virtual void setRPMSignal(const float speed) = 0;
	virtual uint32_t getCounter() const = 0;
	virtual float getSpeedRPM() const = 0;
	virtual uint16_t getControlSignalMax() const = 0;
	virtual float getRPMSignalMin() const = 0;
	virtual float getRPMSignalMax() const = 0;
	virtual void setDistance(const int distance) = 0;
};

#endif /* IMOTOR_H_ */
