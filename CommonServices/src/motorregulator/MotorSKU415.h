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

	class SKU415Config final
	{
	public:
		friend class MotorSKU415;

		explicit SKU415Config(const float minRPM, const float maxRPM, const float rpmVsCtrlSignalSlope,
			const float rpmVsCtrlSignalYIntercept) : m_minRPM(minRPM), m_maxRPM(maxRPM), m_rpmVsCtrlSignalSlope(rpmVsCtrlSignalSlope),
			m_rpmVsCtrlSignalYIntercept(rpmVsCtrlSignalYIntercept)
		{
		}

	private:
		float m_minRPM;
		float m_maxRPM;
		float m_rpmVsCtrlSignalSlope;
		float m_rpmVsCtrlSignalYIntercept;
	};

	explicit MotorSKU415(MotorDriver8833::Motor& motor, const SKU415Config& config);

	void coast() override;
	void stop() override;
	void setControlSignal(const bool forward, const uint16_t signal) override;
	void setRPMSignal(const float speed) override;
	float getSpeedRPM() const override;
	float getLowSpeedRPM() const;
	float getHighSpeedRPM() const;
	bool isLowSpeedValid() const;
	bool isLowSpeedActive() const;

	uint32_t getCounter() const override;

	uint16_t getControlSignalMax() const override;
	float getRPMSignalMin() const override;
	float getRPMSignalMax() const override;
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
	SKU415Config m_config;

	DistanceTracker m_distanceTracker;
};

#endif /* MOTORSKU415_H_ */
