/*
 * Publisher.h
 *
 *  Created on: 21 aug 2016
 *      Author: Thomas
 */

#ifndef TBOX_PUBLISHER_H_
#define TBOX_PUBLISHER_H_

// standard
#include <algorithm>
#include <functional>
#include <vector>

// project
#include "exceptionMacros.h"

namespace tbox
{
	template<typename T>
	class Publisher
	{
	public:

		Publisher() : m_subscribers()
		{
		}

		virtual ~Publisher()
		{
		}

		void subscribe(T& subscriber)
		{
			TB_ASSERT(std::find(m_subscribers.begin(), m_subscribers.end(), &subscriber) == m_subscribers.end());

			m_subscribers.push_back(&subscriber);

			subscriber.setPublisher(*this, m_subscribers.size() - 1);
		}

		void unsubscribe(T& subscriber)
		{
			for (std::size_t i = 0; i < m_subscribers.size(); ++i)
			{
				if (m_subscribers.at(i) == &subscriber)
				{
					m_subscribers.at(i) = nullptr;
					break;
				}
			}
		}

		void panicUnsubscribe(const std::size_t subscriberId)
		{
			m_subscribers.at(subscriberId) = nullptr;
		}

	protected:

		template <typename... Args>
		void notifySubscribers(std::function<void(T&, Args...)> notifier, Args... args)
		{
			for (std::size_t i = 0; i < m_subscribers.size(); ++i)
			{
				if (m_subscribers.at(i) != nullptr)
				{
					notifier(*m_subscribers.at(i), args...);
				}
			}
		}

	private:

		std::vector<T*> m_subscribers;
	};
}

#endif /* TBOX_PUBLISHER_H_ */
