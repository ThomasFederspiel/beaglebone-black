/*
 * IRunnable.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_IRUNNABLE_H_
#define TBOX_IRUNNABLE_H_

// standard
#include <string>

#include "SchedulerUtil.h"

class IRunnable
{
public:

	class IScheduler
	{
	public:
		enum SchedulerPolicy
		{
			POLICY_NORMAL = SchedulerUtil::POLICY_NORMAL,
			POLICY_FIFO = SchedulerUtil::POLICY_FIFO,
			POLICY_RR = SchedulerUtil::POLICY_RR,
			POLICY_BATCH = SchedulerUtil::POLICY_BATCH,
			POLICY_IDLE = SchedulerUtil::POLICY_IDLE,
			POLICY_DEADLINE = SchedulerUtil::POLICY_DEADLINE
		};

		virtual void setPolicy(SchedulerPolicy policy, int priority) = 0;
		virtual void setPriority(int priority) = 0;
		virtual SchedulerPolicy getPolicy() const = 0;
		virtual int getPriority() const = 0;

		static const char* toString(const SchedulerPolicy policy)
		{
			#define CASE(v) case v: return #v;
			switch (policy)
			{
				CASE(POLICY_NORMAL)
				CASE(POLICY_FIFO)
				CASE(POLICY_RR)
				CASE(POLICY_BATCH)
				CASE(POLICY_IDLE)
				CASE(POLICY_DEADLINE)
			}
			static std::string tmp = std::to_string(static_cast<int>(policy));
			return tmp.c_str();
			#undef CASE
		}
	};

	IRunnable() = default;

	virtual ~IRunnable() = default;

	IRunnable(const IRunnable&) = delete;
	IRunnable& operator=(const IRunnable&) = delete;

	virtual void setScheduler(IScheduler* const scheduler) = 0;

	virtual const std::string& name() const = 0;
	virtual void operator()() = 0;
};

#endif /* TBOX_IRUNNABLE_H_ */
