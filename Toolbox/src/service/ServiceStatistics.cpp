/*
 * ServiceStatistics.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "ServiceStatistics.h"

// standard

// local

// project
#include "Logger.h"
#include "stdExtension.h"
#include "tboxdefs.h"
#include "Utils.h"

MODULE_LOG(ServiceStatistics);

namespace
{
static ServiceStatistics::duration_t to_duration_t(const ServiceStatistics::duration_nano_t& duration)
{
	return std::chrono::round<ServiceStatistics::duration_t>(duration);
}
}

ServiceStatistics::ServiceStatistics(const std::string& name) : ServiceStatistics(name, false)
{
}

ServiceStatistics::ServiceStatistics(const std::string& name, const bool holdoff) : m_name(name), m_holdoffActive(holdoff),
	m_holdoffTime(), m_messageStat()
{
}

ServiceStatistics::ServiceStatistics(const std::string& name, const std::chrono::milliseconds& holdoffDuration) : m_name(name),
	m_holdoffActive(true), m_holdoffTime(std::chrono::steady_clock::now() + holdoffDuration), m_messageStat()
{
}

const std::string& ServiceStatistics::name() const
{
	return m_name;
}

std::size_t ServiceStatistics::getMessageCount() const
{
	std::size_t count = 0;

	for (const auto e : m_messageStat)
	{
		count += e.second.count();
	}

	return count;
}

void ServiceStatistics::clearHoldoff()
{
	m_holdoffActive = false;
}

const ServiceStatistics::messageStatisticsMap_t& ServiceStatistics::getMessageStatistics() const
{
	return m_messageStat;
}

void ServiceStatistics::update(const ServiceMessageBase& msg, const duration_t& duration, const ServiceStatistics::duration_t& latency,
		const std::size_t depth)
{
	if (m_holdoffActive)
	{
		if (m_holdoffTime > std::chrono::steady_clock::now())
		{
			return;
		}

		m_holdoffActive = false;
	}

	MessageStatistics* stat = nullptr;

	const auto id = msg.getType();
	const auto periodic = msg.isPeriodic();

	auto iter = m_messageStat.find(id);

	if (iter != m_messageStat.end())
	{
		stat = &iter->second;
	}
	else
	{
		auto item = m_messageStat.emplace(std::piecewise_construct,
		          std::forward_as_tuple(id),
		          std::forward_as_tuple(id, periodic));

		stat = &item.first->second;
	}

	stat->update(duration, latency, depth);
}

ServiceStatistics::MessageStatistics::MessageStatistics(const MessageType_t id, const bool periodic) : m_id(id), m_periodic(periodic),
	m_minMaxDuration(), m_averageVarianceDuration(), m_minMaxPeriod(), m_averageVariancePeriod(),
	m_minMaxLatency(), m_averageVarianceLatency(), m_lastUpdateTime()
{
}

void ServiceStatistics::MessageStatistics::update(const duration_t& duration, const ServiceStatistics::duration_t& latency,
	const std::size_t depth)
{
	m_minMaxQueueDepth.add(depth);
	m_averageVarianceQueueDepth.add(depth);

	m_minMaxDuration.add(duration.count());
	m_averageVarianceDuration.add(duration.count());

	m_minMaxLatency.add(latency.count());
	m_averageVarianceLatency.add(latency.count());

	if (!isPeriodic())
	{
		return;
	}

	if (m_lastUpdateTime.time_since_epoch().count() > 0)
	{
		const auto now = std::chrono::steady_clock::now();
		const auto period = to_duration_t(now - m_lastUpdateTime);
		m_lastUpdateTime = now;

		m_minMaxPeriod.add(period.count());
		m_averageVariancePeriod.add(period.count());
	}
	else
	{
		m_lastUpdateTime = std::chrono::steady_clock::now();
	}
}

ServiceStatistics::queuedepth_t ServiceStatistics::MessageStatistics::lastQueueDepth() const
{
	return m_averageVarianceQueueDepth.last();
}

ServiceStatistics::queuedepth_t ServiceStatistics::MessageStatistics::averageQueueDepth() const
{
	return m_averageVarianceQueueDepth.average();
}

ServiceStatistics::queuedepth_t ServiceStatistics::MessageStatistics::varianceQueueDepth() const
{
	return m_averageVarianceQueueDepth.variance();
}

ServiceStatistics::queuedepth_t ServiceStatistics::MessageStatistics::stdDeviationQueueDepth() const
{
	return m_averageVarianceQueueDepth.stdDeviation();
}

ServiceStatistics::queuedepth_t ServiceStatistics::MessageStatistics::minQueueDepth() const
{
	return m_minMaxQueueDepth.min();
}

ServiceStatistics::queuedepth_t ServiceStatistics::MessageStatistics::maxQueueDepth() const
{
	return m_minMaxQueueDepth.max();
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::lastDuration() const
{
	return createChronoDuration(m_averageVarianceDuration.last());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::averageDuration() const
{
	return createChronoDuration(m_averageVarianceDuration.average());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::varianceDuration() const
{
	return createChronoDuration(m_averageVarianceDuration.variance());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::stdDeviationDuration() const
{
	return createChronoDuration(m_averageVarianceDuration.stdDeviation());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::minDuration() const
{
	return createChronoDuration(m_minMaxDuration.min());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::maxDuration() const
{
	return createChronoDuration(m_minMaxDuration.max());
}


ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::lastPeriod() const
{
	return createChronoDuration(m_averageVariancePeriod.last());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::averagePeriod() const
{
	return createChronoDuration(m_averageVariancePeriod.average());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::variancePeriod() const
{
	return createChronoDuration(m_averageVariancePeriod.variance());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::stdDeviationPeriod() const
{
	return createChronoDuration(m_averageVariancePeriod.stdDeviation());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::minPeriod() const
{
	return createChronoDuration(m_minMaxPeriod.min());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::maxPeriod() const
{
	return createChronoDuration(m_minMaxPeriod.max());
}


ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::lastLatency() const
{
	return createChronoDuration(m_averageVarianceLatency.last());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::averageLatency() const
{
	return createChronoDuration(m_averageVarianceLatency.average());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::varianceLatency() const
{
	return createChronoDuration(m_averageVarianceLatency.variance());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::stdDeviationLatency() const
{
	return createChronoDuration(m_averageVarianceLatency.stdDeviation());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::minLatency() const
{
	return createChronoDuration(m_minMaxLatency.min());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::maxLatency() const
{
	return createChronoDuration(m_minMaxLatency.max());
}

ServiceStatistics::duration_t ServiceStatistics::MessageStatistics::createChronoDuration(const duration_t::rep value) const
{
	return std::chrono::duration<duration_t::rep, duration_t::period>(value);
}

ServiceStatisticsWrapper::ServiceStatisticsWrapper(const std::string& name) : m_statistics(name), m_mutex()
{
}

ServiceStatisticsWrapper::ServiceStatisticsWrapper(const std::string& name, const bool holdoff) : m_statistics(name, holdoff), m_mutex()
{
}

ServiceStatisticsWrapper::ServiceStatisticsWrapper(const std::string& name, const std::chrono::milliseconds& holdoffDuration) : m_statistics(name, holdoffDuration), m_mutex()
{
}

// Thread safe
ServiceStatistics ServiceStatisticsWrapper::clone() const
{
	std::unique_lock<std::mutex> lock(m_mutex);

	return m_statistics;
}

void ServiceStatisticsWrapper::clearHoldoff()
{
	m_statistics.clearHoldoff();
}

void ServiceStatisticsWrapper::update(const ServiceMessageBase& msg, const ServiceStatistics::duration_nano_t& duration,
	const ServiceStatistics::duration_nano_t& latency, const std::size_t depth)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	m_statistics.update(msg, to_duration_t(duration), to_duration_t(latency), depth);
}

