/*
 * ThreadWrapper.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_THREADWRAPPER_H_
#define TBOX_THREADWRAPPER_H_

// standard
#include <atomic>
#include <memory>
#include <string>
#include <thread>

// linux
#include <sys/types.h>

// local
#include "IRunnable.h"
#include "Publisher.h"
#include "Subscriber.h"

class ProcTaskStat;
class ThreadFactory;
class ThreadWrapper;

class IThreadWrapperSubscriber : public tbox::Subscriber<ThreadWrapper, IThreadWrapperSubscriber>
{
public:

	virtual ~IThreadWrapperSubscriber() = default;

	virtual void onTerminated(ThreadWrapper& thread) = 0;
};

class ThreadWrapper final : public tbox::Publisher<IThreadWrapperSubscriber>, IRunnable::IScheduler
{
public:

	class ThreadContext final
	{
	public:
		ThreadContext();
		~ThreadContext();
		void set(ThreadWrapper* const thread);

	private:
		ThreadWrapper* m_thread;
	};

	explicit ThreadWrapper(IRunnable& runnable, ThreadFactory& factory);

	ThreadWrapper(const ThreadWrapper&) = delete;
	ThreadWrapper operator=(const ThreadWrapper& rhs ) = delete;

	void start();
	const std::string& name() const;

	void operator()();

	// Interface IRunner::IScheduler
	void setPolicy(IRunnable::IScheduler::SchedulerPolicy policy, int priority) override;
	void setPriority(int priority) override;
	SchedulerPolicy getPolicy() const override;
	int getPriority() const override;

	pid_t getTid() const
	{
		return m_tid;
	}

	float getCpuUsage();

private:
	void setScheduler();
	void onTerminated();
	void notifyTerminated();

	IRunnable& m_runnable;

	ThreadFactory& m_threadFactory;

	std::unique_ptr<std::thread> m_thread;
	std::atomic<pid_t> m_tid;
	mutable std::unique_ptr<ProcTaskStat> m_taskStat;
};

#endif /* TBOX_THREADWRAPPER_H_ */
