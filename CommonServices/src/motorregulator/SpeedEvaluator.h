/*
 * SpeedEvaluator.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef SPEEDEVALUTOR_H_
#define SPEEDEVALUTOR_H_

// standard
#include <cmath>

// local

// project
#include "exceptionMacros.h"
#include "IPCDeviceProxyEventEQEP.h"
#include "pru_eqep_types.hp"

namespace
{
static constexpr std::size_t InitialCount = 2;
}

class SpeedEvaluator final
{
public:
	explicit SpeedEvaluator(const float quadraturePulsesPerRev, const float lowSpeedRPMLimit) :
			m_prevEQep(), m_lastEQep(), m_quadraturePulsesPerRev(quadraturePulsesPerRev),
			m_lowSpeedPPMSLimit(fromRPMtoPPMS(lowSpeedRPMLimit)),
			m_initialized(InitialCount), m_nofPulsesHigh(0.f), m_lowSpeedIsValid(false),
			m_lowSpeedPPMS(0.f), m_highSpeedPPMS(0.f)
	{
	}

	void update(const IPCDeviceProxyEventEQEP& eqep)
	{
		m_prevEQep = m_lastEQep;
		m_lastEQep = eqep;
		m_lowSpeedIsValid = eqep.isLowSpeedValid();

		if (m_initialized == 0)
		{
			calcPPMSSpeeds();
		}
		else
		{
			--m_initialized;
		}
	}

	void calcPPMSSpeeds()
	{
		m_nofPulsesHigh = static_cast<int64_t>(m_lastEQep.getCapCounter()) - m_prevEQep.getCapCounter();

		m_lowSpeedPPMS = calcLowSpeedPPMS(m_nofPulsesHigh);
		m_highSpeedPPMS = calcHighSpeedPPMS(m_nofPulsesHigh);
	}

	float calcLowSpeedPPMS(float nofPulsesHigh) const
	{
		float pp_ms = 0.f;

		if (m_lowSpeedIsValid)
		{
			// low speed
			const float nofPulsesLow = m_lastEQep.getUEventPulses();
			const uint64_t timeDelta_ns = m_lastEQep.getCapPeriod() * m_lastEQep.getCaptureTimeTick();

			TB_ASSERT(timeDelta_ns != 0);

			pp_ms = (1000000 * nofPulsesLow) / timeDelta_ns;

			if ((m_lastEQep.getCapStatus() & EQEP_STS_QUADRATURE_DIR_FLAG) == 0)
			{
				pp_ms = -pp_ms;
			}
		}

		return pp_ms;
	}

	float calcHighSpeedPPMS(float nofPulsesHigh) const
	{
		// high speed
		const uint32_t timeDelta_ms = m_lastEQep.getUnitTime();

		TB_ASSERT(timeDelta_ms != 0);

		return nofPulsesHigh / timeDelta_ms;
	}

	float getSpeedPPMS() const
	{
		float pp_ms = 0.f;

		if (isLowSpeedActive())
		{
			pp_ms = m_lowSpeedPPMS;
		}
		else
		{
			pp_ms = m_highSpeedPPMS;
		}

		return pp_ms;
	}

	float getLowSpeedRPM() const
	{
		return fromPPMStoRPM(m_lowSpeedPPMS);
	}

	bool isLowSpeedValid() const
	{
		return m_lowSpeedIsValid;
	}

	bool isLowSpeedActive() const
	{
		return isLowSpeedValid() && (std::fabs(m_lowSpeedPPMS) < m_lowSpeedPPMSLimit);
	}

	float getHighSpeedRPM() const
	{
		return fromPPMStoRPM(m_highSpeedPPMS);
	}

	float getSpeedRPM() const
	{
		return fromPPMStoRPM(getSpeedPPMS());
	}

private:

	float fromRPMtoPPMS(const float rpm) const
	{
		return rpm * m_quadraturePulsesPerRev / (1000.f * 60.f);
	}

	float fromPPMStoRPM(const float ppms) const
	{
		return (1000.f * 60.f * ppms) / m_quadraturePulsesPerRev;
	}

	IPCDeviceProxyEventEQEP m_prevEQep;
	IPCDeviceProxyEventEQEP m_lastEQep;
	float m_quadraturePulsesPerRev;
	float m_lowSpeedPPMSLimit;
	EQepCapClkDivisor m_clkDiv;
	std::size_t m_initialized;
	float m_nofPulsesHigh;
	bool m_lowSpeedIsValid;
	float m_lowSpeedPPMS;
	float m_highSpeedPPMS;
};

#endif /* SPEEDEVALUTOR_H_ */
