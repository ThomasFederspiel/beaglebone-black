/*
 * PIDRegulator.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef PIDREGULATOR_H_
#define PIDREGULATOR_H_

// standard
#include <chrono>
#include <limits>

// local

// project
#include "exceptionMacros.h"
#include "TMath.h"
#include "Timer.h"

// ;+
//#define LOG
#ifdef LOG
#include <iostream>
#endif

class IMotor;

template <typename Dur>
class PIDRegulator final
{
public:
	typedef Dur value_type;

	enum TuningModes
	{
		RegularTuning = 0x00,
		DerviateOnMeasurment = 0x01,
		ProportionalOnMeasurment = 0x02
	};

	class PIDState final
	{
	public:
		explicit PIDState() :
			m_setPoint(0.f), m_kpFactor(0.f), m_kiFactor(0.f), m_kdFactor(0.f),
			m_lastInput(0.f), m_lastError(0.f), m_lastOutput(0.f)
		{
		}

		float m_setPoint;
		float m_kpFactor;
		float m_kiFactor;
		float m_kdFactor;
		float m_lastInput;
		float m_lastError;
		float m_lastOutput;
	};

	explicit PIDRegulator(const Dur& intervall, const float kp = 0.f, const float ki = 0.f, const float kd = 0.f) :
		m_timer(intervall), m_pBias(0.f), m_kp(0.f), m_ki(0.f), m_kd(0.f), m_enabled(false),
		m_pidState(), m_mode(RegularTuning),
		m_outputMax(std::numeric_limits<float>::max()), m_outputMin(std::numeric_limits<float>::min()),
		m_initialize(false), m_reverseActingProcess(false)
	{
		setTuning(kp, ki, kd);
	}

	const PIDState& getState() const
	{
		return m_pidState;
	}

	void setEnable(const bool enable)
	{
		if (!m_enabled && enable)
		{
			m_initialize = true;
		}

		m_enabled = enable;
	}

	bool isEnabled() const
	{
		return m_enabled;
	}

	float getPoint() const
	{
		return m_pidState.m_setPoint;
	}

	void setPoint(const float setPoint)
	{
		m_pidState.m_setPoint = setPoint;

		updatePBias();
    }

	float update(const float input)
	{
		bool overrun = false;
		Dur overrunTime;

		return update(input, overrun, overrunTime);
	}

	float update(const float input, bool& overrun)
	{
		Dur overrunTime;

		return update(input, overrun, overrunTime);
	}

	float update(const float input, bool& overrun, Dur& overrunTime)
	{
		if (!m_enabled)
		{
			return m_pidState.m_lastOutput;
		}

		if (m_timer.hasTriggered(overrun, overrunTime))
		{
			if (m_initialize)
			{
				m_pidState.m_lastInput = input;
				m_pidState.m_kpFactor = 0;
				m_pidState.m_kiFactor = m_pidState.m_lastOutput;

				m_initialize = false;
			}

			const float error = m_pidState.m_setPoint - input;

			// ;+
#ifdef LOG
			std::cout << "m_setPoint = " << m_pidState.m_setPoint << std::endl;
//			std::cout << "input = " << input << std::endl;
			std::cout << "error = " << error << std::endl;
#endif
			if (m_mode & ProportionalOnMeasurment)
			{
				const float inputError = input - m_pidState.m_lastInput;
				m_pidState.m_kpFactor -= m_kp * inputError;

				limitValue(m_pidState.m_kpFactor);
			}
			else
			{
				m_pidState.m_kpFactor = m_kp * error;
			}

			m_pidState.m_kiFactor += m_ki * error;

			limitValue(m_pidState.m_kiFactor);

			if (m_mode & DerviateOnMeasurment)
			{
				const float derivateInput = input - m_pidState.m_lastInput;
				m_pidState.m_kdFactor = -m_kd * derivateInput;
			}
			else
			{
				const float derivateError = error - m_pidState.m_lastError;
				m_pidState.m_kdFactor = m_kd * derivateError;
			}

			// ;+
#ifdef LOG
			std::cout << "m_pBias = " << m_pBias << std::endl;
			std::cout << "m_pidState.m_kpFactor = " << m_pidState.m_kpFactor << std::endl;
			std::cout << "m_pidState.m_kiFactor = " << m_pidState.m_kiFactor << std::endl;
			std::cout << "m_pidState.m_kdFactor = " << m_pidState.m_kdFactor << std::endl;
#endif

			m_pidState.m_lastInput = input;
			m_pidState.m_lastOutput = m_pidState.m_kpFactor + m_pidState.m_kiFactor + m_pidState.m_kdFactor
					+ m_pBias;

			limitValue(m_pidState.m_lastOutput);

#ifdef LOG
// ;+			std::cout << "m_pidState.m_lastOutput = " << m_pidState.m_lastOutput << std::endl;
#endif

			m_pidState.m_lastError = error;
		}

		return m_pidState.m_lastOutput;
	}

	void setTuning(const float kp, const float ki, const float kd)
	{
		if ((kp < 0.f) && (ki < 0.f) && (kd < 0.f))
		{
			return;
		}

		const auto sampleTimeInSec = m_timer.template getIntervall<std::chrono::duration<float>>().count();
		TB_ASSERT(sampleTimeInSec > 0);

		m_kp = kp;
		m_ki = ki * sampleTimeInSec;
		m_kd = kd / sampleTimeInSec;

		if (m_reverseActingProcess)
		{
			m_kp = 0 - m_kp;
			m_ki = 0 - m_ki;
			m_kd = 0 - m_kd;
		}

		updatePBias();
	}

	void setOutputLimits(const float min, const float max)
	{
		TB_ASSERT(max > min);
		m_outputMin = min;
		m_outputMax = max;
	}

	void setReversActingProcess(const bool enable)
	{
		m_reverseActingProcess = enable;
	}

	void setMode(const TuningModes mode)
	{
		m_mode = mode;
	}

private:

	void limitValue(float& value) const
	{
		if (value > m_outputMax)
		{
			value = m_outputMax;
		}
		else if (value < m_outputMin)
		{
			value = m_outputMin;
		}
	}

	void updatePBias()
	{
		if (tbox::Math::isZero(m_ki))
		{
			m_pBias = m_pidState.m_setPoint;
		}
		else
		{
			m_pBias = 0.f;
		}
	}

	Timer<Dur> m_timer;

	float m_pBias;
	float m_kp;
	float m_ki;
	float m_kd;
	bool m_enabled;
	PIDState m_pidState;
	TuningModes m_mode;
	float m_outputMax;
	float m_outputMin;
	bool m_initialize;
	bool m_reverseActingProcess;
};

#endif /* PIDREGULATOR_H_ */
