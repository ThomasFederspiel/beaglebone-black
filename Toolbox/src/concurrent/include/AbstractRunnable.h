/*
 * AbstractRunnable.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_ABSTRACTRUNNABLE_H_
#define TBOX_ABSTRACTRUNNABLE_H_

// standard
#include <string>

// local
#include "IRunnable.h"

namespace
{
static constexpr IRunnable::IScheduler::SchedulerPolicy DefaultPolicy = IRunnable::IScheduler::POLICY_NORMAL;
static constexpr int DefaultPriority = 0;
}

class AbstractRunnable : public IRunnable
{
public:

	AbstractRunnable() : m_scheduler(nullptr), m_policy(DefaultPolicy), m_priority(DefaultPriority)
	{
	}

	void setScheduler(IScheduler* const scheduler)
	{
		m_scheduler = scheduler;

		if ((m_policy != DefaultPolicy) || (m_priority != DefaultPriority))
		{
			m_scheduler->setPolicy(m_policy, m_priority);
		}
	}

protected:

	void setPriority(const int priority)
	{
		m_priority = priority;

		if (m_scheduler)
		{
			m_scheduler->setPriority(m_priority);
		}
	}

	void setPolicy(const IScheduler::SchedulerPolicy policy, const int priority)
	{
		m_policy = policy;
		m_priority = priority;

		if (m_scheduler)
		{
			m_scheduler->setPolicy(m_policy, m_priority);
		}
	}

private:
	IScheduler*	m_scheduler;

	IScheduler::SchedulerPolicy m_policy;
	int m_priority;
};

#endif /* TBOX_ABSTRACTRUNNABLE_H_ */
