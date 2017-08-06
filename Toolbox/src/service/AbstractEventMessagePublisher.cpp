/*
 * AbstractService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "AbstractEventMessagePublisher.h"

// standard

// local
#include "IMessageReceiver.h"
#include "ServiceMessageBase.h"

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "tboxdefs.h"

MODULE_LOG(AbstractEventMessagePublisher);

AbstractEventMessagePublisher::AbstractEventMessagePublisher() : m_messageSubscriberMap()
{
}

void AbstractEventMessagePublisher::registerEvent(const MessageType_t messageType)
{
	TB_ASSERT(m_messageSubscriberMap.find(messageType) == m_messageSubscriberMap.end());

	(void)m_messageSubscriberMap[messageType];
}

void AbstractEventMessagePublisher::publishEvent(const ServiceMessageBase& message)
{
	const auto item = m_messageSubscriberMap.find(message.getType());

	TB_ASSERT(item != m_messageSubscriberMap.end());

	for (const auto& subscribers : item->second)
	{
		subscribers->post(message);
	}
}

void AbstractEventMessagePublisher::subscribeEvent(const MessageType_t messageType, IMessageReceiver& subscriber)
{
	const auto item = m_messageSubscriberMap.find(messageType);

	TB_ASSERT(item != m_messageSubscriberMap.end());

	for (const auto& registered : item->second)
	{
		TB_ASSERT(&subscriber != registered)
	}

	item->second.push_back(&subscriber);
}

void AbstractEventMessagePublisher::unsubscribeEvent(const MessageType_t messageType, const IMessageReceiver& subscriber)
{
	const auto item = m_messageSubscriberMap.find(messageType);

	TB_ASSERT(item != m_messageSubscriberMap.end());

	bool found = false;

	for (std::size_t i = 0; i < item->second.size(); ++i)
	{
		const auto& registered = item->second.at(i);

		if (&subscriber == registered)
		{
			item->second.erase(item->second.begin() + i);
			found = true;
			break;
		}
	}

	TB_ASSERT(found);
}

void AbstractEventMessagePublisher::unsubscribeAllEvents(const IMessageReceiver& subscriber)
{
	bool found = false;

	for (auto& item : m_messageSubscriberMap)
	{
		for (std::size_t i = 0; i < item.second.size(); ++i)
		{
			const auto& registered = item.second.at(i);

			if (&subscriber == registered)
			{
				item.second.erase(item.second.begin() + i);
				found = true;
				break;
			}
		}
	}

	TB_ASSERT(found);
}
