/*
 * IEventMessagePublisher.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_IEVENTMESSAGEPUBLISHER_H_
#define TBOX_IEVENTMESSAGEPUBLISHER_H_

// local
#include "ServiceTypes.h"

class ServiceMessageBase;
class IMessageReceiver;

class IEventMessagePublisher
{
public:

	IEventMessagePublisher() = default;
	virtual ~IEventMessagePublisher() = default;

	virtual void registerEvent(const MessageType_t messageType) = 0;
	virtual void publishEvent(const ServiceMessageBase& message) = 0;
	virtual void subscribeEvent(const MessageType_t messageType, IMessageReceiver& subscriber) = 0;
	virtual void unsubscribeEvent(const MessageType_t messageType, const IMessageReceiver& subscriber) = 0;
	virtual void unsubscribeAllEvents(const IMessageReceiver& subscriber) = 0;

};

#endif /* TBOX_IEVENTMESSAGEPUBLISHER_H_ */
