/*
 * TimeTracker.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_TIMETRACKER_H_
#define TBOX_TIMETRACKER_H_

// standard
#include <chrono>

// project
#include "circular.h"

template <typename Dur, std::size_t CAPACITY = 100>
class TimeTracker final
{
public:

	TimeTracker() : m_buffer(CAPACITY)
	{
	}

	TimeTracker(const TimeTracker&) = delete;
	TimeTracker& operator=(const TimeTracker&) = delete;

	void addPoint()
	{
		auto now = std::chrono::steady_clock::now();
		Dur timeDiff = 0;

		if (!m_buffer.empty())
		{
			timeDiff = now - m_buffer.back().m_time;
		}

		m_buffer.push_back(TimePoint(now, timeDiff));
	}

	void clear()
	{
		m_buffer.clear();
	}

private:

	class TimePoint final
	{
	public:

		explicit TimePoint(std::chrono::steady_clock::time_point& time, Dur& duration) : m_time(time),
			m_duration(duration)
		{
		}

		std::chrono::steady_clock::time_point m_time;
		Dur m_duration;
	};

	circular_buffer<TimePoint> m_buffer;
};

#endif /* TBOX_TIMETRACKER_H_ */
