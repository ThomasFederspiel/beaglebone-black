/*
 * SoundService.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef SOUNDSERVICE_H_
#define SOUNDSERVICE_H_

// standard
#include <memory>
#include <string>

// local

// project
#include "AbstractService.h"
#include "MessageBase.h"
#include "ServiceAllocator.h"
#include "SoundMessage.h"

class IPCDeviceProxyService;
class ServiceMessageBase;
class SoundServiceState;

using namespace melodyservice;

class SoundService final : public AbstractService
{
public:
	explicit SoundService(const std::string& name);

	// Declared as to avoid unique_ptr incomplete type
	~SoundService() ;

protected:

	void onMessage(const ServiceMessageBase& message) override;
	void onStart(ServiceAllocator& allocator) override;
	StopStatus onStop(ServiceAllocator& allocator) override;

private:

	void startPlayerQueue();
	void manageCUICommands(ServiceAllocator& allocator, const bool reg);

	std::shared_ptr<IPCDeviceProxyService> m_ipcProxyService;
	std::unique_ptr<SoundServiceState> m_stateMachine;
};

#endif /* SOUNDSERVICE_H_ */
