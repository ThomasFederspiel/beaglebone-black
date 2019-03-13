/*
 * Timer.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_TIMER_H_
#define TBOX_TIMER_H_

// standard
#include <chrono>
#include <cmath>

// project

template <typename Dur>
class Timer final
{
public:

	Timer(const Dur& intervall, const float tolerance = 0.1f) : m_lastTriggerTime(std::chrono::steady_clock::now()),
		m_intervall(intervall), m_tolerance(tolerance * intervall.count())
	{
	}

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

	template <typename CDur>
	CDur getIntervall() const
	{
		return std::chrono::duration_cast<CDur>(m_intervall);
	}

	void setPoint()
	{
		m_lastTriggerTime = std::chrono::steady_clock::now();
	}

	bool hasTriggered()
	{
		bool overrun = false;
		return hasTriggered(overrun);
	}

	bool hasTriggered(bool& overrun)
	{
		Dur overrunTime;

		return hasTriggered(overrun, overrunTime);
	}

	bool hasTriggered(bool& overrun, Dur& overrunTime)
	{
		bool triggered = false;

		overrun = false;

		const auto now = std::chrono::steady_clock::now();
		const auto timePassed = std::chrono::duration_cast<Dur>(now - m_lastTriggerTime);

		const auto diff = std::abs(timePassed.count() - m_intervall.count());

		if (diff <= m_tolerance)
		{
			m_lastTriggerTime = now;
			triggered = true;
		}
		else if (timePassed.count() > m_intervall.count())
		{
			m_lastTriggerTime = now;
			triggered = true;

			overrun = true;
			overrunTime = timePassed - m_intervall;
		}

		return triggered;
	}

private:

	std::chrono::steady_clock::time_point m_lastTriggerTime;
	Dur m_intervall;
	float m_tolerance;
};

#endif /* TBOX_TIMER_H_ */
