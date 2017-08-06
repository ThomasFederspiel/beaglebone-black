/*
 * WorkerThreadManager.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "WorkerThreadManager.h"

// project
#include "exceptionMacros.h"
#include "IRunnable.h"
#include "Logger.h"
#include "tboxdefs.h"

MODULE_LOG(WorkerThreadManager);

WorkerThreadManager::WorkerThreadManager() : m_workerThreads()
{
}

void WorkerThreadManager::onTerminated(ThreadWrapper& thread)
{
	bool found = false;

	std::unique_lock<std::mutex> lock(m_lock);

	for (std::size_t i = 0; i < m_workerThreads.size(); ++i)
	{
		const auto& item = m_workerThreads[i];

		if (item.second.get() == &thread)
		{
			TB_ASSERT(item.first);

			INFO("Worker thread " << item.first->name() << " has terminated");

			m_workerThreads.erase(m_workerThreads.begin() + i);

			found = true;

			break;
		}
	}

	TB_ASSERT(found);
}

void WorkerThreadManager::createWorker(IWorkerRunnable& runnable)
{
	auto thread = ThreadFactory::instance().createThread(runnable);

	thread->subscribe(*this);

	{
		std::unique_lock<std::mutex> lock(m_lock);

		m_workerThreads.push_back(std::make_pair(&runnable, thread));
	}

	thread->start();
}

void WorkerThreadManager::stopWorkers()
{
	std::unique_lock<std::mutex> lock(m_lock);

	for (const auto& item : m_workerThreads)
	{
		TB_ASSERT(item.first);

		item.first->stop();
	}
}

void WorkerThreadManager::stopWorker(IWorkerRunnable& runnable)
{
	std::unique_lock<std::mutex> lock(m_lock);

	for (const auto& item : m_workerThreads)
	{
		TB_ASSERT(item.first);

		if (item.first == &runnable)
		{
			item.first->stop();

			break;
		}
	}
}

bool WorkerThreadManager::isWorkerActive(IWorkerRunnable& runnable) const
{
	std::unique_lock<std::mutex> lock(m_lock);

	for (const auto& item : m_workerThreads)
	{
		TB_ASSERT(item.first);

		if (item.first == &runnable)
		{
			return true;
		}
	}

	return false;
}

bool WorkerThreadManager::hasActiveWorkers() const
{
	std::unique_lock<std::mutex> lock(m_lock);

	return !m_workerThreads.empty();
}

const WorkerThreadManager::WorkerThreads& WorkerThreadManager::getWorkers() const
{
	return m_workerThreads;
}

