/*
 * ThreadUtil.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "ThreadUtil.h"

// system
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

void ThreadUtil::setThreadName(pthread_t thread, const std::string& name)
{
	pthread_setname_np(thread, name.c_str());
}

pid_t ThreadUtil::gettid()
{
	#ifdef SYS_gettid
	return syscall(SYS_gettid);
	#else
	#error "SYS_gettid unavailable on this system"
	#endif
}

pid_t ThreadUtil::getpid()
{
	return getpid();
}
