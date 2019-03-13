/*
 * SchedulerUtil.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "SchedulerUtil.h"

// standard
#include <fstream>
#include <string>

// system
#include <sys/utsname.h>

// project
#include "Logger.h"

MODULE_LOG(SchedulerUtil)

namespace
{
static const std::string PreemtRtTag = "PREEMPT RT";
static const std::string RealtimeSysPath = "/sys/kernel/realtime";
}

bool SchedulerUtil::supportRRScheduler(const bool log)
{
    struct utsname u;

	bool crit1 = false;
	bool crit2 = false;

    const auto err = uname(&u);

    if (err == 0)
    {
		crit1 = std::string(u.version).find(PreemtRtTag) != std::string::npos;

		std::ifstream file(RealtimeSysPath);

		if (file)
		{
			int flag = 0;

			file >> flag;

	        crit2 = flag == 1;
		}
    }

    if (log)
    {
		INFO("Kernel type is " << ((crit1 && crit2) ? "PREEMPT RT" : "vanilla"));
    }

    return crit1 && crit2;
}

int SchedulerUtil::setThreadScheduler(const SchedulerPolicy& policy, int priority)
{
     pthread_t this_thread = pthread_self();

     struct sched_param params =
     {
         priority
     };

     return pthread_setschedparam(this_thread, policy, &params);
}

int SchedulerUtil::getThreadPolicy(SchedulerPolicy& policy)
{
    return getThreadPolicy(pthread_self(), policy);
}

int SchedulerUtil::getThreadPolicy(pthread_t thread, SchedulerPolicy& policy)
{
    struct sched_param params = {};

    int tmpPolicy = POLICY_FIFO;
    const auto ret = pthread_getschedparam(thread, &tmpPolicy, &params);

    policy = static_cast<SchedulerPolicy>(tmpPolicy);

    return ret;
}

int SchedulerUtil::setThreadPriority(const int priority)
{
    pthread_t this_thread = pthread_self();

	return pthread_setschedprio(this_thread, priority);
}

int SchedulerUtil::getThreadPriority(int& priority)
{
    return getThreadPriority(pthread_self(), priority);
}

int SchedulerUtil::getThreadPriority(pthread_t thread, int& priority)
{
    int policy = POLICY_FIFO;

    struct sched_param params = {};

    const auto ret = pthread_getschedparam(thread, &policy, &params);

    priority = params.sched_priority;

    return ret;
}

int SchedulerUtil::getPriorityMin(const SchedulerPolicy& policy)
{
	return sched_get_priority_min(policy);
}

int SchedulerUtil::getPriorityMax(const SchedulerPolicy& policy)
{
	return sched_get_priority_max(policy);
}

