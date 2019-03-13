/*
 * AtomicQueue.h
 *
 *  Created on: 4 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_ATOMICQUEUE_H_
#define TBOX_ATOMICQUEUE_H_

// standard
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

// project
#include "exceptionMacros.h"

namespace tbox
{
template <typename T>
class AtomicQueue final
{
private:

	using queue_t = std::queue<T>;

public:

	class GetStatus
	{
	public:

		enum class Status
		{
			Timeout = 0,
			Data
		};

		GetStatus(std::queue<T>* const queue = nullptr) :  m_queue(queue)
		{
		}

		Status status() const
		{
			if (m_queue)
			{
				return Status::Data;
			}

			return Status::Timeout;
		}

		T getData()
		{
			TB_ASSERT(m_queue);

			T item = std::move(m_queue->front());

			m_queue->pop();

			return std::move(item);
		}

	private:
		std::queue<T>* const m_queue;
	};

	AtomicQueue();

	AtomicQueue(const AtomicQueue&) = delete;
	AtomicQueue& operator=(const AtomicQueue&) = delete;

	void put(const T& item);
	void put(T&& item);
	T get();
	typename queue_t::size_type size() const;

	template <typename Rep, typename Period>
	GetStatus get(const std::chrono::duration<Rep, Period>& duration);

	void clear();

private:
	queue_t m_queue;
	mutable std::mutex m_lock;
	std::condition_variable m_signal;
};

template <typename T>
AtomicQueue<T>::AtomicQueue() : m_queue(), m_lock(), m_signal()
{
}

template <typename T>
void AtomicQueue<T>::put(const T& item)
{
	std::unique_lock<std::mutex> lock(m_lock);

	m_queue.push(item);

	m_signal.notify_one();
}

template <typename T>
void AtomicQueue<T>::put(T&& item)
{
	std::unique_lock<std::mutex> lock(m_lock);

	m_queue.push(std::move(item));

	m_signal.notify_one();
}

template <typename T>
void AtomicQueue<T>::clear()
{
	std::unique_lock<std::mutex> lock(m_lock);

	// clear
	m_queue = std::queue<T>();
}

template <typename T>
T AtomicQueue<T>::get()
{
	std::unique_lock<std::mutex> lock(m_lock);

	m_signal.wait(lock, [&](){ return !m_queue.empty(); });

	T item = std::move(m_queue.front());

	m_queue.pop();

	return std::move(item);
}

template <typename T>
typename AtomicQueue<T>::queue_t::size_type AtomicQueue<T>::size() const
{
	return m_queue.size();
}

template <typename T>
template <typename Rep, typename Period>
typename AtomicQueue<T>::GetStatus AtomicQueue<T>::get(const std::chrono::duration<Rep, Period>& duration)
{
	std::unique_lock<std::mutex> lock(m_lock);

	if (m_signal.wait_for(lock, duration, [&](){ return !m_queue.empty(); }))
	{
		return GetStatus(&m_queue);
	}

	return GetStatus();
}
} // namespace tbox

#endif /* TBOX_ATOMICQUEUE_H_ */
