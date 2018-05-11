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

template <typename Dur>
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

	typename Dur::rep duration() const
	{
		return std::chrono::duration_cast<Dur>(std::chrono::steady_clock::now() - m_startTime).count();
	}

private:

	std::chrono::steady_clock::time_point m_startTime;
	std::chrono::steady_clock::time_point m_mark;
};

#endif /* TBOX_STOPWATCH_H_ */
