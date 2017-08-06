/*
 * StopWatch.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_STOPWATCH_H_
#define TBOX_STOPWATCH_H_

// standard
#include <chrono>

class StopWatch final
{
public:

	StopWatch() : m_startTime(), m_mark()
	{
	}

	StopWatch(const StopWatch&) = delete;
	StopWatch& operator=(const StopWatch&) = delete;

	void start()
	{
		m_startTime = std::chrono::steady_clock::now();
	}

	void mark()
	{
		m_mark = std::chrono::steady_clock::now();
	}

	std::chrono::milliseconds::rep durationAsMs() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration()).count();
	}

	std::chrono::microseconds::rep durationAsUs()  const
	{
		return duration().count();
	}

	std::chrono::milliseconds::rep markDurationAsMs()  const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(m_mark - m_startTime).count();
	}

	std::chrono::microseconds::rep markDurationAsUs()  const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(m_mark - m_startTime).count();
	}

private:

	std::chrono::microseconds duration() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_startTime);
	}

	std::chrono::steady_clock::time_point m_startTime;
	std::chrono::steady_clock::time_point m_mark;
};

#endif /* TBOX_STOPWATCH_H_ */
