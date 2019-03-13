/*
 * Signal.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_TBSIGNAL_H_
#define TBOX_TBSIGNAL_H_

// standard
#include <chrono>
#include <condition_variable>

namespace tbox
{
class Signal final
{
public:

	Signal() : m_trigger(false), m_lock(), m_cv()
	{
	}

	void notify()
	{
		std::lock_guard<std::mutex> lock(m_lock);
		m_trigger = true;

		m_cv.notify_all();
	}

	template <class Rep, class Period>
	bool wait_for(const std::chrono::duration<Rep, Period>& rel_time)
	{
		std::unique_lock<std::mutex> lock(m_lock);

		if (m_cv.wait_for(lock, rel_time, [this] { return m_trigger; }))
		{
			m_trigger = false;

			// Trigger
			return true;
		}
		else
		{
			// Timeout
			return false;
		}
	}

	void wait()
	{
		std::unique_lock<std::mutex> lock(m_lock);

		m_cv.wait(lock, [this] { return m_trigger; });

		m_trigger = false;
	}

private:
	bool m_trigger;
	std::mutex m_lock;
	std::condition_variable m_cv;
};
}

#endif /* TBOX_TBSIGNAL_H_ */
