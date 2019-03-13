/*
 * SchedulerUtil.h
 *
 *  Created on: 26 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_SCHEDULERUTIL_H_
#define TBOX_SCHEDULERUTIL_H_

#include <linux/sched.h>

#include <pthread.h>

class SchedulerUtil final
{
public:

	enum SchedulerPolicy
	{
		POLICY_NORMAL = SCHED_NORMAL,
		POLICY_FIFO = SCHED_FIFO,
		POLICY_RR = SCHED_RR,
		POLICY_BATCH = SCHED_BATCH,
		POLICY_IDLE = SCHED_IDLE,
		POLICY_DEADLINE = SCHED_DEADLINE
	};

	static bool supportRRScheduler(bool log);

	static int setThreadScheduler(const SchedulerPolicy& policy, int priority);
	static int getThreadPolicy(SchedulerPolicy& policy);
	static int getThreadPolicy(pthread_t thread, SchedulerPolicy& policy);
	static int setThreadPriority(int priority);
	static int getThreadPriority(int& priority);
	static int getThreadPriority(pthread_t thread, int& priority);

	static int getPriorityMin(const SchedulerPolicy& policy);
	static int getPriorityMax(const SchedulerPolicy& policy);

};

#endif /* TBOX_SCHEDULERUTIL_H_ */
