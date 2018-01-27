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
#include <string>

// local
#include "SpeedEvaluator.h"

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

		enum class Decay
		{
			Slow = 0,
			Fast
		};

		explicit Motor(IPCDeviceProxyService& proxy, const PwmssDeviceEnum pwmssDevice);

		void close();
		void coast();
		void open();
		void stop();
		void setDecay(const Decay decay);
		void setControlSignal(const bool forward, const uint16_t signal);
		PwmssDeviceEnum getPwmssDevice() const;
		uint16_t getControlSignalMax() const;

	private:
		enum class State
		{
			Stopped = 0,
			Coasting,
			Forward,
			Reverse
		};

		void selectChannels(const bool forward);

		static const char* toString(const State state)
		{
			#define CASE(v) case v: return #v;
			switch (state)
			{
				CASE(State::Stopped)
				CASE(State::Coasting)
				CASE(State::Forward)
				CASE(State::Reverse)
			}
			static std::string tmp = std::to_string(static_cast<int>(state));
			return tmp.c_str();
			#undef CASE
		}

		IPCDeviceEPwmProxy m_epwmProxy;
		IPCDevicePwmsProxy m_pwmsProxy;
		PwmssDeviceEnum m_pwmssDevice;

		State m_state;
		uint16_t m_pwmPeriod;
		uint16_t m_pwmDuty;
		bool m_pwmEnabled;
		Decay m_decay;
		EPwmChannelEnum m_directionChannel;
		EPwmChannelEnum m_pwmChannel;
	};

	explicit MotorDriver8833(IPCDeviceProxyService& proxy,
			const PwmssDeviceEnum leftMotorPwmDevice, const PwmssDeviceEnum rightMotorPwmDevice,
			const IPCDeviceGpioProxy::GpioPins nSleep, const IPCDeviceGpioProxy::GpioPins nFault);

	void close()
	{
		m_leftMotor.close();
		m_rightMotor.close();
	}

	void open()
	{
		m_leftMotor.open();
		m_rightMotor.open();
	}

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
