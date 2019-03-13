/*
 * ThreadFactory.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "ThreadFactory.h"

// standard
#include <chrono>
#include <iomanip>
#include <mutex>

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "ThreadWrapper.h"
#include "stdExtension.h"
#include "ProcTaskStat.h"

MODULE_LOG(ThreadFactory);

namespace
{
static constexpr std::chrono::milliseconds THREAD_TERMINATION_WAIT_TIME_MS = std::chrono::milliseconds(100);

static std::once_flag m_onceFlag;

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
		logActiveThreads();
	}

	TB_ASSERT(m_threads.empty());
}

ThreadFactory& ThreadFactory::instance()
{
	std::call_once(m_onceFlag, []()
	{
		s_instance = std::make_unique<ThreadFactory>();
	});

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

	TB_ERROR("Unknown thread died");
}

void ThreadFactory::streamThreadInformation(std::ostream& stream) const
{
	std::unique_lock<std::mutex> lock(m_mutex);

	std::size_t nameSize = 0;

	for (const auto& thread : m_threads)
	{
		if (nameSize < thread->name().size())
		{
			nameSize = thread->name().size();
		}
	}

	stream << std::left << std::setw(nameSize + 3) << "Name" << std::setw(10) << "Tid" << std::setw(18) << "Scheduler" << std::setw(10)
		<< "Priority" << std::setw(10) << "CPU Usage [%]" << std::endl;

	for (const auto& thread : m_threads)
	{
		const auto policy = thread->getPolicy();
		const auto priority = thread->getPriority();
		const auto tid = thread->getTid();
		const auto usage = thread->getCpuUsage();

		stream << std::left << std::setw(nameSize + 3) << thread->name() << std::setw(10) << tid << std::setw(18) << IRunnable::IScheduler::toString(policy)
			<< std::setw(10) << priority << std::setw(10) << usage << std::endl;
	}
}

void ThreadFactory::logActiveThreads() const
{
	INFO("Active threads");

	std::unique_lock<std::mutex> lock(m_mutex);

	for (const auto& thread : m_threads)
	{
		INFO(" " << thread->name());
	}
}


