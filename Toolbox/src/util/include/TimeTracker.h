/*
 * TimeTracker.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_TIMETRACKER_H_
#define TBOX_TIMETRACKER_H_

// standard
#include <algorithm>
#include <chrono>
#include <limits>
#include <vector>

// project
#include "Circular.h"

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
		Dur timeDiff(0);

		if (!m_buffer.empty())
		{
			timeDiff = std::chrono::duration_cast<Dur>(now - m_buffer.back().m_time);

			if ((m_buffer.size() == 1) && (m_buffer.back().m_duration.count() == 0))
			{
				m_buffer.clear();
			}
		}

		m_buffer.push_back(TimePoint(now, timeDiff));
	}

	void clear()
	{
		m_buffer.clear();
	}

	bool full() const
	{
		return m_buffer.size() == m_buffer.capacity();
	}

	Dur getMedian()
	{
		Dur median(0);

		if (m_buffer.size() == 1)
		{
			median = m_buffer.back().m_duration;
		}
		else if (m_buffer.size() > 1)
		{
			std::vector<TimePoint> collection(m_buffer.begin(), m_buffer.end());

			std::sort(collection.begin(), collection.end(), [](TimePoint a, TimePoint b)
			{
				return a.m_duration > b.m_duration;
			});

			if (collection.size() % 2 == 0)
			{
				median = (collection.at(collection.size() / 2).m_duration
						+ collection.at(collection.size() / 2 + 1).m_duration) / 2;
			}
			else
			{
				median = collection.at(collection.size() / 2 + 1).m_duration;
			}
		}

		return median;
	}

	Dur getMean()
	{
		Dur sum(0);

		for (const auto& point : m_buffer)
		{
			sum += point.m_duration;
		}

		return sum / m_buffer.size();
	}

	Dur getMax()
	{
		Dur max(0);

		for (const auto& point : m_buffer)
		{
			if (max < point.m_duration)
			{
				max = point.m_duration;
			}
		}

		return max;
	}

	Dur getMin()
	{
		Dur min(std::numeric_limits<typename Dur::rep>::max());

		for (const auto& point : m_buffer)
		{
			if (min > point.m_duration)
			{
				min = point.m_duration;
			}
		}

		return min;
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
#include <chrono>
