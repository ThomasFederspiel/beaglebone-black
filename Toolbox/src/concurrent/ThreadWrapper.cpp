/*
 * ThreadWrapper.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "ThreadWrapper.h"

// standard
#include <exception>
#include <mutex>
#include <string>

// local
#include "ThreadFactory.h"

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "ProcTaskStat.h"
#include "SchedulerUtil.h"
#include "stdExtension.h"
#include "ThreadUtil.h"
#include "Utils.h"

MODULE_LOG(ThreadWrapper);

namespace
{
thread_local ThreadWrapper::ThreadContext m_threadContext;
}

ThreadWrapper::ThreadContext::ThreadContext() : m_thread(nullptr)
{
}

ThreadWrapper::ThreadContext::~ThreadContext()
{
	TB_ASSERT(m_thread);

	m_thread->onTerminated();
}

void ThreadWrapper::ThreadContext::set(ThreadWrapper* const thread)
{
	m_thread = thread;
}

ThreadWrapper::ThreadWrapper(IRunnable& runnable, ThreadFactory& factory) : m_runnable(runnable), m_threadFactory(factory), m_tid(0),
	m_taskStat()
{
}

void ThreadWrapper::onTerminated()
{
	m_thread->detach();

	notifyTerminated();

	m_threadFactory.onThreadDied(*this);
}

const std::string& ThreadWrapper::name() const
{
	return m_runnable.name();
}

void ThreadWrapper::start()
{
	m_thread = std::make_unique<std::thread>(&ThreadWrapper::operator(), this);
}

void ThreadWrapper::notifyTerminated()
{
	notifySubscribers(std::function<void(IThreadWrapperSubscriber& subscriber, ThreadWrapper* wrapper)>([](IThreadWrapperSubscriber& subscriber,
				ThreadWrapper* wrapper)
			{
				subscriber.onTerminated(*wrapper);
			}), this);
}

void ThreadWrapper::setPolicy(IScheduler::SchedulerPolicy policy, int priority)
{
	const auto ret = SchedulerUtil::setThreadScheduler(static_cast<SchedulerUtil::SchedulerPolicy>(policy),
		priority);

	TB_ASSERT(ret == 0);
}

void ThreadWrapper::setPriority(int priority)
{
	const auto ret = SchedulerUtil::setThreadPriority(priority);

	TB_ASSERT(ret == 0);
}

IRunnable::IScheduler::SchedulerPolicy ThreadWrapper::getPolicy() const
{
	SchedulerUtil::SchedulerPolicy policy = SchedulerUtil::POLICY_NORMAL;

	const auto ret = SchedulerUtil::getThreadPolicy(m_thread->native_handle(), policy);

	TB_ASSERT(ret == 0);

	return static_cast<IScheduler::SchedulerPolicy>(policy);
}

int ThreadWrapper::getPriority() const
{
	int priority = 0;

	const auto ret = SchedulerUtil::getThreadPriority(m_thread->native_handle(), priority);

	TB_ASSERT(ret == 0);

	return priority;
}

float ThreadWrapper::getCpuUsage()
{
	static std::mutex mutex;

	if (m_taskStat)
	{
		std::lock_guard<std::mutex> guard(mutex);

		const auto& stat = m_taskStat->getTaskUsage();

		return stat.cpu_usage;
	}

	return 0.f;
}

void ThreadWrapper::operator()()
{
	m_threadContext.set(this);
	m_runnable.setScheduler(this);

	m_tid = ThreadUtil::gettid();
	m_taskStat = std::make_unique<ProcTaskStat>(m_tid);

	ThreadUtil::setThreadName(m_thread->native_handle(), m_runnable.name());

	try
	{
		m_runnable();
	}
	catch (std::exception& e)
	{
		ERROR("Exception killed thread " << name() << ", what: " << e.what());
	}
	catch (...)
	{
		ERROR("Unknown exception killed thread " << name());
	}

	m_runnable.setScheduler(nullptr);
}
