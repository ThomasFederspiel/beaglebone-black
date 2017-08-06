/*
 * ThreadWrapper.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "ThreadWrapper.h"

// standard
#include <exception>
#include <string>

// local
#include "ThreadFactory.h"

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "tboxdefs.h"

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

ThreadWrapper::ThreadWrapper(IRunnable& runnable, ThreadFactory& factory) : m_runnable(runnable), m_threadFactory(factory)
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
	m_thread = tbox::make_unique<std::thread>(&ThreadWrapper::operator(), this);
}

void ThreadWrapper::notifyTerminated()
{
	notifySubscribers(std::function<void(IThreadWrapperSubscriber& subscriber, ThreadWrapper* wrapper)>([](IThreadWrapperSubscriber& subscriber,
				ThreadWrapper* wrapper)
			{
				subscriber.onTerminated(*wrapper);
			}), this);
}

void ThreadWrapper::operator()()
{
	m_threadContext.set(this);

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
}
