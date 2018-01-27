/*
 * SpeedEvaluator.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef SPEEDEVALUTOR_H_
#define SPEEDEVALUTOR_H_

// standard

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
	explicit SpeedEvaluator(const float quadraturePulsesPerRev, const float quadratureLowSpeedPercentage) :
			m_prevEQep(), m_lastEQep(), m_quadraturePulsesPerRev(quadraturePulsesPerRev),
			m_quadratureLowSpeedLimit(quadraturePulsesPerRev * quadratureLowSpeedPercentage),
			m_initialized(InitialCount), m_lowSpeedIsValid(false)
	{
	}

	void update(const IPCDeviceProxyEventEQEP& eqep)
	{
		m_prevEQep = m_lastEQep;
		m_lastEQep = eqep;
		m_lowSpeedIsValid = eqep.isLowSpeedValid();

		if (m_initialized != 0)
		{
			--m_initialized;
		}
	}

	float getSpeedPPMS() const
	{
		float pp_ms = 0.f;

		if (m_initialized == 0)
		{
			const float nofPulsesHigh = static_cast<int64_t>(m_lastEQep.getCapCounter()) - m_prevEQep.getCapCounter();

			if ((nofPulsesHigh < m_quadratureLowSpeedLimit) && m_lowSpeedIsValid)
			{
				// low speed
				const float nofPulsesLow = m_lastEQep.getUEventPulses();
				const uint64_t timeDelta_ns = m_lastEQep.getCapPeriod() * m_lastEQep.getCaptureTimeTick();

				pp_ms = (1000000 * nofPulsesLow) / timeDelta_ns;

				if ((m_lastEQep.getCapStatus() & EQEP_STS_QUADRATURE_DIR_FLAG) == 0)
				{
					pp_ms = -pp_ms;
				}
			}
			else
			{
				// high speed
				const uint32_t timeDelta_ms = m_lastEQep.getUnitTime();

				pp_ms = nofPulsesHigh / timeDelta_ms;
			}
		}

		return pp_ms;
	}

	float getSpeedRPM() const
	{
		return (1000.f * 60.f * getSpeedPPMS()) / m_quadraturePulsesPerRev;
	}

private:

	IPCDeviceProxyEventEQEP m_prevEQep;
	IPCDeviceProxyEventEQEP m_lastEQep;
	float m_quadraturePulsesPerRev;
	float m_quadratureLowSpeedLimit;
	EQepCapClkDivisor m_clkDiv;
	std::size_t m_initialized;
	bool m_lowSpeedIsValid;
};

#endif /* SPEEDEVALUTOR_H_ */
