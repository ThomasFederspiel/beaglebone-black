/*
 * StreamManager.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef STREAMMANAGER_H_
#define STREAMMANAGER_H_

// standard
#include <map>
#include <memory>
#include <vector>

// project
#include "AbstractService.h"
#include "CommonMessageTypes.h"
#include "FrameProtocol.h"
#include "StreamSelectMessage.h"

class ServiceAllocator;
class ServiceMessageBase;

using namespace commonservices;

class StreamManager final
{
public:

	StreamManager(AbstractService& service);

	void start(ServiceAllocator& allocator);
	void stop(ServiceAllocator& allocator);

	void addStream(FrameProtocolConnection& connection, const StreamSelectMessage& message);
	void process(const ServiceMessageBase& message);

private:

	class StreamServiceContext final
	{
	public:

		explicit StreamServiceContext(const StreamSelectMessage::StreamId streamId,
				const CommonMessageTypes::Type messageType,
				std::shared_ptr<AbstractService> service);

		StreamSelectMessage::StreamId m_streamId;
		CommonMessageTypes::Type m_messageType;
		std::shared_ptr<AbstractService> m_service;
	};

	using Connections = std::vector<FrameProtocolConnection>;
	using ActiveStreamMap = std::map<CommonMessageTypes::Type, Connections>;

	CommonMessageTypes::Type getSelectedMessage(const StreamSelectMessage& message) const;
	void subscribeEvent(const CommonMessageTypes::Type type);
	void unsubscribeEvent(const CommonMessageTypes::Type type);

	AbstractService& m_service;
	ActiveStreamMap m_activeStreams;
	std::vector<StreamServiceContext> m_streamContexts;
};

#endif /* STREAMMANAGER_H_ */
