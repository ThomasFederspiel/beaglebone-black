/*
 * StreamService.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef STREAMSERVICE_H_
#define STREAMSERVICE_H_

// standard
#include <memory>
#include <string>

// project
#include "AbstractService.h"
#include "ServiceAllocator.h"

template<typename ComServerImpl>
class ProtocolServer;
class ServiceMessageBase;
class TCPServer;

class StreamService final : public AbstractService
{
public:

	explicit StreamService(const std::string& name);

	~StreamService();

protected:

	void onMessage(const ServiceMessageBase& message) override;
	void onStart(ServiceAllocator& allocator) override;
	StopStatus onStop(ServiceAllocator& allocator) override;

private:
	std::unique_ptr<ProtocolServer<TCPServer>> m_protocolServer;
};

#endif /* STREAMSERVICE_H_ */
