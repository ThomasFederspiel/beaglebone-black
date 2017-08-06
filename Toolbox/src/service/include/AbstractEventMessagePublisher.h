/*
 * AbstractEventMessagePublisher.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_ABSTRACTEVENTMESSAGEPUBLISHER_H_
#define TBOX_ABSTRACTEVENTMESSAGEPUBLISHER_H_

// standard
#include <map>
#include <vector>

// local
#include "IEventMessagePublisher.h"
#include "ServiceTypes.h"

class ServiceMessageBase;
class IMessageReceiver;

class AbstractEventMessagePublisher : public IEventMessagePublisher
{
public:

	AbstractEventMessagePublisher();

	void registerEvent(const MessageType_t messageType) override;
	void publishEvent(const ServiceMessageBase& message) override;
	void subscribeEvent(const MessageType_t messageType, IMessageReceiver& subscriber) override;
	void unsubscribeEvent(const MessageType_t messageType, const IMessageReceiver& subscriber) override;
	void unsubscribeAllEvents(const IMessageReceiver& subscriber) override;

private:
	using MessageSubscribers = std::vector<IMessageReceiver*>;
	using MessageSubscriberMap = std::map<MessageType_t, MessageSubscribers>;

	MessageSubscriberMap m_messageSubscriberMap;
};

#endif /* TBOX_ABSTRACTEVENTMESSAGEPUBLISHER_H_ */
