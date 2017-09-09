/*
 * ProtocolManager.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef PROTOCOLMANAGER_H_
#define PROTOCOLMANAGER_H_

// standard
#include <cstdint>
#include <functional>
#include <memory>

// project
#include "AbstractService.h"
#include "Callboard.h"
#include "ComSwitchboard.h"
#include "FrameProtocol.h"
#include "IComStreamMessage.h"
#include "StreamManager.h"

class BinaryComSerializer;

template<typename ComServerImpl>
class ProtocolServer;


class ServiceAllocator;
class ServiceMessageBase;
class StreamSelectMessage;
class TCPServer;

class ProtocolManager final : public IFrameProtocolSubscriber
{
public:

	ProtocolManager(AbstractService& service);
	~ProtocolManager();

	void start(ServiceAllocator& allocator);
	void stop(ServiceAllocator& allocator);

	void process(const ServiceMessageBase& message);

private:

	void registerProtocolMessages();
	void onReceivedFrame(FrameProtocolConnection& connection, const uint8_t* const data, const std::size_t length) override;

	void handleSelectMessage(FrameProtocolConnection& connection, const StreamSelectMessage& message);

	std::unique_ptr<ProtocolServer<TCPServer>> m_protocolServer;
	tbox::ComSwitchboard<IComStreamMessage::MessageIds, FrameProtocolConnection&, BinaryComSerializer&> m_switchboard;

	StreamManager m_streamManager;
};

#endif /* PROTOCOLMANAGER_H_ */
