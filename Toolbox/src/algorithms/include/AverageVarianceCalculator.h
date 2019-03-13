/*
 * AverageVarianceCalculator.h
 *
 *  Created on: 21 jan 2019
 *      Author: Thomas
 */

#ifndef AVERAGEVARIANCECALCULATOR_H_
#define AVERAGEVARIANCECALCULATOR_H_

#include <cmath>

/**
 * http://datagenetics.com/blog/november22017/index.html
 */

template<typename T>
class AverageVarianceCalculator
{
public:

	AverageVarianceCalculator() : m_last(), m_averageCount(0), m_average(0.f), m_variance(0.f)
	{
	}

	void add(const T value)
	{
		m_last = value;

		float prevAverage = m_average;

		++m_averageCount;
		m_average = prevAverage + (value - prevAverage) / m_averageCount;

		m_variance = m_variance + (value - prevAverage) * (value - m_average);
	}

	std::size_t count() const
	{
		return m_averageCount;
	}

	T average() const
	{
		return static_cast<T>(m_average);
	}

	T variance() const
	{
		return static_cast<T>(m_variance / count());
	}

	T stdDeviation() const
	{
		return static_cast<T>(std::sqrt(variance()));
	}

	T last() const
	{
		return m_last;
	}

private:
	T m_last;
	std::size_t m_averageCount;
	float m_average;
	float m_variance;
};

#endif /* AVERAGEVARIANCECALCULATOR_H_ */
