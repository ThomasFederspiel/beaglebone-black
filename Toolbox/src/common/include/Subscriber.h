/*
 * Subscriber.h
 *
 *  Created on: 21 aug 2016
 *      Author: Thomas
 */

#ifndef TBOX_SUBSCRIBER_H_
#define TBOX_SUBSCRIBER_H_

#include "Publisher.h"

namespace tbox
{
	template<typename P, typename S>
	class Subscriber
	{
	public:
		friend class Publisher<S>;

		Subscriber() : m_publisher(nullptr), m_subscriberId(0)
		{
		}

		virtual ~Subscriber()
		{
			if (m_publisher)
			{
				m_publisher->panicUnsubscribe(m_subscriberId);
			}
		}

	private:

		void setPublisher(Publisher<S>& publisher, const std::size_t subscriberId)
		{
			m_publisher = &publisher;
			m_subscriberId = subscriberId;
		}

		Publisher<S>* m_publisher;
		std::size_t m_subscriberId;
	};
}

#endif /* TBOX_SUBSCRIBER_H_ */
