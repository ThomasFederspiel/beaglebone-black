/*
 * MotorDriver8833.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef MOTORDRIVER8833_H_
#define MOTORDRIVER8833_H_

// standard
#include <cstdint>

// local

// project
#include "IPCDeviceEPwmProxy.h"
#include "IPCDeviceGpioProxy.h"
#include "IPCDevicePwmsProxy.h"
#include "pru_epwm_types.hp"
#include "pru_pwms_types.hp"

class IPCDeviceProxyService;

class MotorDriver8833 final
{
public:
	friend class Motor;

	class Motor final
	{
	public:

		explicit Motor(IPCDeviceProxyService& proxy, const PwmDeviceEnum pwmDevice);

		void open();
		void close();
		void setSpeed(const float speed);
		float getSpeed() const;
		void stop();
		void coast();

	private:
		void setDirection(const bool forward);

		IPCDeviceEPwmProxy m_epwmProxy;
		IPCDevicePwmsProxy m_pwmsProxy;

		uint16_t m_pwmPeriod;
		bool m_forward;
		bool m_pwmEnabled;
	};

	explicit MotorDriver8833(IPCDeviceProxyService& proxy,
			const PwmDeviceEnum leftMotorPwmDevice, const PwmDeviceEnum rightMotorPwmDevice,
			const IPCDeviceGpioProxy::GpioPins nSleep, const IPCDeviceGpioProxy::GpioPins nFault);

	void setSleepMode(const bool enable);
	bool isFaulty() const;

	Motor& getLeftMotor()
	{
		return m_leftMotor;
	}

	Motor& getRightMotor()
	{
		return m_rightMotor;
	}

private:
	Motor m_leftMotor;
	Motor m_rightMotor;

	IPCDeviceGpioProxy m_gpioProxy;

	IPCDeviceGpioProxy::GpioPins m_nSleep;
	IPCDeviceGpioProxy::GpioPins m_nFault;

};

#endif /* MOTORDRIVER8833_H_ */
