/*
 * ServiceStatistics.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_SERVICESTATISTICS_H_
#define TBOX_SERVICESTATISTICS_H_

// standard
#include <chrono>
#include <map>
#include <mutex>

// local
#include "ServiceTypes.h"

// project
#include "AverageVarianceCalculator.h"
#include "MinMaxTracker.h"
#include "ServiceMessageBase.h"

class ServiceStatistics final
{
public:
	using duration_t = std::chrono::microseconds;
	using duration_nano_t = std::chrono::nanoseconds;

	using queuedepth_t = std::size_t;

	class MessageStatistics final
	{
	public:

		MessageStatistics(const MessageType_t id, const bool periodic);

		MessageType_t id() const
		{
			return m_id;
		}

		bool isPeriodic() const
		{
			return m_periodic;
		}

		std::size_t count() const
		{
			return m_averageVarianceDuration.count();
		}

		queuedepth_t lastQueueDepth() const;
		queuedepth_t averageQueueDepth() const;
		queuedepth_t varianceQueueDepth() const;
		queuedepth_t stdDeviationQueueDepth() const;
		queuedepth_t minQueueDepth() const;
		queuedepth_t maxQueueDepth() const;

		duration_t lastDuration() const;
		duration_t averageDuration() const;
		duration_t varianceDuration() const;
		duration_t stdDeviationDuration() const;
		duration_t minDuration() const;
		duration_t maxDuration() const;

		duration_t lastPeriod() const;
		duration_t averagePeriod() const;
		duration_t variancePeriod() const;
		duration_t stdDeviationPeriod() const;
		duration_t minPeriod() const;
		duration_t maxPeriod() const;

		duration_t lastLatency() const;
		duration_t averageLatency() const;
		duration_t varianceLatency() const;
		duration_t stdDeviationLatency() const;
		duration_t minLatency() const;
		duration_t maxLatency() const;

		void update(const duration_t& duration, const ServiceStatistics::duration_t& latency, const std::size_t depth);

	private:

		ServiceStatistics::duration_t createChronoDuration(const duration_t::rep value) const;

		MessageType_t m_id;
		bool m_periodic;

		MinMaxTracker<queuedepth_t> m_minMaxQueueDepth;
		AverageVarianceCalculator<queuedepth_t> m_averageVarianceQueueDepth;

		MinMaxTracker<duration_t::rep> m_minMaxDuration;
		AverageVarianceCalculator<duration_t::rep> m_averageVarianceDuration;

		MinMaxTracker<duration_t::rep> m_minMaxPeriod;
		AverageVarianceCalculator<duration_t::rep> m_averageVariancePeriod;

		MinMaxTracker<duration_t::rep> m_minMaxLatency;
		AverageVarianceCalculator<duration_t::rep> m_averageVarianceLatency;

		std::chrono::steady_clock::time_point m_lastUpdateTime;
	};

	using messageStatisticsMap_t = std::map<MessageType_t, MessageStatistics>;

	ServiceStatistics(const std::string& name);
	ServiceStatistics(const std::string& name, bool holdoff);
	ServiceStatistics(const std::string& name, const std::chrono::milliseconds& holdoffDuration);

	const std::string& name() const;

	std::size_t getMessageCount() const;

	const messageStatisticsMap_t& getMessageStatistics() const;

	void clearHoldoff();

	void update(const ServiceMessageBase& msg, const duration_t& duration, const ServiceStatistics::duration_t& latency,
			const std::size_t depth);

private:
	duration_t createChronoDuration(const duration_t::rep value) const;

	std::string m_name;
	bool m_holdoffActive;
	std::chrono::steady_clock::time_point m_holdoffTime;
	messageStatisticsMap_t m_messageStat;
};

class ServiceStatisticsWrapper final
{
public:

	ServiceStatisticsWrapper(const std::string& name);
	ServiceStatisticsWrapper(const std::string& name, bool holdoff);
	ServiceStatisticsWrapper(const std::string& name, const std::chrono::milliseconds& holdoffDuration);

	// Thread safe
	ServiceStatistics clone() const;

	void clearHoldoff();

	// Thread safe
	void update(const ServiceMessageBase& msg, const ServiceStatistics::duration_nano_t& duration, const ServiceStatistics::duration_nano_t& latency,
			const std::size_t depth);

private:
	ServiceStatistics m_statistics;
	mutable std::mutex m_mutex;
};

#endif /* TBOX_SERVICESTATISTICS_H_ */
