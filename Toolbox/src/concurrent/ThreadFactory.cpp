/*
 * ThreadFactory.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "ThreadFactory.h"

// standard
#include <chrono>

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "ThreadWrapper.h"
#include "tboxdefs.h"

MODULE_LOG(ThreadFactory);

namespace
{
static constexpr std::chrono::milliseconds THREAD_TERMINATION_WAIT_TIME_MS = std::chrono::milliseconds(100);
} // end namespace

std::unique_ptr<ThreadFactory> ThreadFactory::s_instance;

ThreadFactory::ThreadFactory() : m_mutex(), m_signal(), m_threads()
{
}

ThreadFactory::~ThreadFactory()
{
	waitOnThreadTerminations();

	if (!m_threads.empty())
	{
		logThreads();
	}

	TB_ASSERT(m_threads.empty());
}

ThreadFactory& ThreadFactory::instance()
{
	if (!s_instance)
	{
		s_instance = tbox::make_unique<ThreadFactory>();
	}

	return *s_instance;
}

void ThreadFactory::waitOnThreadTerminations()
{
	std::unique_lock<std::mutex> lock(m_mutex);

	if (!m_threads.empty())
	{
		if (!m_signal.wait_for(lock, THREAD_TERMINATION_WAIT_TIME_MS, [&](){ return m_threads.empty(); }))
		{
			ERROR("Timed out waiting for termination of threads");
		}
	}
}

ThreadFactory::thread_t& ThreadFactory::createThread(IRunnable& runnable)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	thread_t thread = std::make_shared<ThreadWrapper>(runnable, *this);

	m_threads.push_back(thread);

	return m_threads.back();
}

void ThreadFactory::onThreadDied(const ThreadWrapper& thread)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	for (std::size_t i = 0; i < m_threads.size(); ++i)
	{
		if (m_threads.at(i).get() == &thread)
		{
			TB_ASSERT(m_threads.at(i)->name() == thread.name());

			m_threads.erase(m_threads.begin() + i);

			INFO("Thread " << thread.name() << " has stopped");

			m_signal.notify_one();

			return;
		}
	}

	TB_ERROR("Unkown thread died");
}

void ThreadFactory::logThreads() const
{
	INFO("Active threads");

	for (const auto& thread : m_threads)
	{
		INFO(" " << thread->name());
	}
}


