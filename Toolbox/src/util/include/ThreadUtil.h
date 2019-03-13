/*
 * ThreadUtil.h
 *
 *  Created on: 26 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_THREADUTIL_H_
#define TBOX_THREADUTIL_H_

// standard
#include <string>

// system
#include <pthread.h>

class ThreadUtil final
{
public:

	static pid_t gettid();
	static pid_t getpid();
	static void setThreadName(pthread_t thread, const std::string& name);
};

#endif /* TBOX_THREADUTIL_H_ */
