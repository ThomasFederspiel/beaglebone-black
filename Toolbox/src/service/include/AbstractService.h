/*
 * AbstractService.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_ABSTRACTSERVICE_H_
#define TBOX_ABSTRACTSERVICE_H_

// standard
#include <chrono>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// local
#include "IMessageReceiver.h"
#include "IMessageService.h"
#include "ServiceStatistics.h"

// project
#include "AtomicQueue.h"
#include "AbstractRunnable.h"
#include "ThreadFactory.h"
#include "ThreadWrapper.h"
#include "WorkerThreadManager.h"

class ServiceAllocator;
class ServiceMessageBase;

class AbstractService : public IMessageService, public AbstractRunnable
{
private:

	static constexpr uint8_t StopTries = 5;
	static constexpr std::chrono::milliseconds StopWaitDelay = std::chrono::milliseconds(50);

public:
	friend class ServiceManager;

	enum class ServiceState : int
	{
		Created = 0,
		Running,
		Stopping,
		Stopped
	};

	static constexpr std::chrono::milliseconds StopTimeout = StopTries * StopWaitDelay;

	// Thread safe
	const std::string& name() const override;

	// Thread safe
	ServiceStatistics getStatistics();

	// Thread safe
	template<class Rep, class Period>
	bool waitFor(const ServiceState state,
			const std::chrono::duration<Rep, Period>& time) const;

	// Thread safe
	ServiceState state() const;

	// Thread safe
	void post(const ServiceMessageBase& message) override;

	// Not thread safe
	void start(ServiceAllocator& allocator) override;
	void stop(ServiceAllocator& allocator) override;
	void signalServicesReady() override;

	static const char* toString(const ServiceState state)
	{
		#define CASE(v) case v: return #v;
		switch (state)
		{
			CASE(ServiceState::Created)
			CASE(ServiceState::Running)
			CASE(ServiceState::Stopping)
			CASE(ServiceState::Stopped)
		}
		static std::string tmp = std::to_string(static_cast<int>(state));
		return tmp.c_str();
		#undef CASE
	}

protected:

	enum class StopStatus
	{
		Done = 0,
		Pending
	};

	explicit AbstractService(const std::string&);

	// Called on service thread
	virtual void onMessage(const ServiceMessageBase& message) = 0;

	// Called on service thread
	virtual void onStart(ServiceAllocator& allocator) = 0;

	// Called on service thread
	virtual StopStatus onStop(ServiceAllocator& allocator) = 0;

	// Called when all services has begun to process messages
	// Called on service thread
	virtual void onServicesReadyMessage(ServiceMessageBase& message)
	{
	}

	virtual void onStopStarted()
	{
	}

	bool isStopRequested() const;

	void createWorker(WorkerThreadManager::AbstractWorkerRunnable& runnable);
	void stopWorker(WorkerThreadManager::AbstractWorkerRunnable& runnable);
	void startWorker(WorkerThreadManager::AbstractWorkerRunnable& runnable);
	bool isWorkerActive(WorkerThreadManager::AbstractWorkerRunnable& runnable);

private:

	void signalStopStarted();
	void waitOnStart();
	void onSystemMessage(ServiceMessageBase& message);
	void onStopMessage(ServiceMessageBase& message);
	void onAsyncTaskMessage(ServiceMessageBase& message);

	void operator()() override;

	// Thread safe
	void postInternal(const ServiceMessageBase& message);
	void setState(ServiceState state);

	std::string m_name;
	ServiceState m_state;
	std::thread::id m_threadId;
	tbox::AtomicQueue<std::unique_ptr<ServiceMessageBase>> m_queue;
	bool m_terminate;
	mutable std::mutex m_lock;
	mutable std::condition_variable m_signal;

	WorkerThreadManager m_workerThreadManager;

	ServiceStatisticsWrapper m_statistics;
};

template<class Rep, class Period>
bool AbstractService::waitFor(const AbstractService::ServiceState state,
		const std::chrono::duration<Rep, Period>& time) const
{
	std::unique_lock<std::mutex> lock(m_lock);

	m_signal.wait_for(lock, time, [&](){ return m_state == state; });

	return m_state == state;
}

#endif /* TBOX_ABSTRACTSERVICE_H_ */
