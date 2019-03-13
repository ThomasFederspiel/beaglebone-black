/*
 * MinMaxTracker.h
 *
 *  Created on: 23 jan 2019
 *      Author: Thomas
 */

#ifndef MINMAXTRACKER_H_
#define MINMAXTRACKER_H_

#include <limits>

template<typename T>
class MinMaxTracker final
{
public:

	MinMaxTracker() : m_last(), m_max(std::numeric_limits<T>::lowest()), m_min(std::numeric_limits<T>::max())
	{
	}

	void add(T value)
	{
		m_last = value;

		if (m_min > value)
		{
			m_min = value;
		}

		if (m_max < value)
		{
			m_max = value;
		}
	}

	T max() const
	{
		return m_max;
	}

	T min() const
	{
		return m_min;
	}

	T last() const
	{
		return m_last;
	}

private:
	T m_last;
	T m_max;
	T m_min;
};

#endif /* MINMAXTRACKER_H_ */
