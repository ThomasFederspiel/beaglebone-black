/*
 * AverageCalculator.h
 *
 *  Created on: 21 jan 2019
 *      Author: Thomas
 */

#ifndef AVERAGECALCULATOR_H_
#define AVERAGECALCULATOR_H_

template<typename T>
class AverageCalculator
{
public:

	AverageCalculator() : m_last(), m_accumulatedCount(0), m_accumulatedSum(),
		m_averageCount(0), m_average(0.f)
	{
	}

	void add(const T value)
	{
		m_last = value;

		const auto sum = m_accumulatedSum + value;

		if (sum < m_accumulatedSum)
		{
			m_average = (m_average * m_averageCount + m_accumulatedSum) / (m_averageCount + m_accumulatedCount);
			m_averageCount += m_accumulatedCount;

			m_accumulatedCount = 1;
			m_accumulatedSum = value;
		}
		else
		{
			++m_accumulatedCount;
			m_accumulatedSum = sum;
		}
	}

	std::size_t count() const
	{
		return m_accumulatedCount + m_averageCount;
	}

	T average() const
	{
		return static_cast<T>((m_average *  m_averageCount + m_accumulatedSum) / count());
	}

	T last() const
	{
		return m_last;
	}

private:
	T m_last;
	std::size_t m_accumulatedCount;
	T m_accumulatedSum;

	std::size_t m_averageCount;
	float m_average;
};

#endif /* AVERAGECALCULATOR_H_ */
