/*
 * MotorSKU415.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef MOTORSKU415_H_
#define MOTORSKU415_H_

// standard
#include <cstdint>

// local
#include "IMotor.h"
#include "MotorDriver8833.h"
#include "SpeedEvaluator.h"

// project

class IPCDeviceProxyEventEQEP;

class MotorSKU415 : public IMotor
{
public:

	explicit MotorSKU415(MotorDriver8833::Motor& motor);

	void coast() override;
	void stop() override;
	void setControlSignal(const bool forward, const uint16_t signal) override;
	void setRPMSignal(const float speed) override;
	float getSpeedRPM() const override;

	uint32_t getCounter() const override;

	uint16_t getControlSignalMax() const override;
	uint16_t getRPMSignalMin() const override;
	uint16_t getRPMSignalMax() const override;
	void setDistance(const int distance) override;

	void update(const IPCDeviceProxyEventEQEP& eqep);

private:

	class DistanceTracker final
	{
	public:
		DistanceTracker();
		void set(const int distance, const int counter);
		bool isEnabled() const;
		bool evaluate(const int counter);

	private:
		int m_distance;
		int m_startCounter;
	};

	uint16_t convertRPM(const float speedRPM) const;

	MotorDriver8833::Motor& m_motor;
	SpeedEvaluator m_speedEvaluator;
	uint32_t m_counter;

	DistanceTracker m_distanceTracker;
};

#endif /* MOTORSKU415_H_ */
