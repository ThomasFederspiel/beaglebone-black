/*
 * WorkerThreadManager.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_WORKERTHREADMANAGER_H_
#define TBOX_WORKERTHREADMANAGER_H_

// standard
#include <mutex>
#include <utility>

// project
#include "IRunnable.h"
#include "ThreadFactory.h"
#include "ThreadWrapper.h"

class IRunnable;

class WorkerThreadManager final : public IThreadWrapperSubscriber
{
public:

	class IWorkerRunnable : public IRunnable
	{
	public:

		// Must allow for call multiple times
		virtual void stop() = 0;
	};

	using WorkerThreads = std::vector<std::pair<IWorkerRunnable*, ThreadFactory::thread_t>>;

	WorkerThreadManager();

	bool hasActiveWorkers() const;

	bool isWorkerActive(IWorkerRunnable& runnable) const;

	const WorkerThreads& getWorkers() const;

	void createWorker(IWorkerRunnable& runnable);

	void stopWorkers();

	void stopWorker(IWorkerRunnable& runnable);

private:
	void onTerminated(ThreadWrapper& thread) override;

	WorkerThreads m_workerThreads;
	mutable std::mutex m_lock;
};

#endif /* TBOX_WORKERTHREADMANAGER_H_ */
