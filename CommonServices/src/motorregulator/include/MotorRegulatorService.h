/*
 * MotorRegulatorService.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef MOTORREGULATORSERVICE_H_
#define MOTORREGULATORSERVICE_H_

// standard
#include <memory>
#include <string>

// local

// project
#include "AbstractService.h"
#include "ServiceAllocator.h"

class IPCDeviceProxyEventEQEP;
class IPCDeviceProxyService;
class MotionMessage;
class MotorDriver8833;
class ServiceMessageBase;

class MotorRegulatorService final : public AbstractService
{
public:

	explicit MotorRegulatorService(const std::string& name);

	~MotorRegulatorService();

protected:

	void onMessage(const ServiceMessageBase& message) override;
	void onStart(ServiceAllocator& allocator) override;
	StopStatus onStop(ServiceAllocator& allocator) override;

private:
	void applyMotion(const MotionMessage& message);
	void manageCUICommands(ServiceAllocator& allocator, const bool register);
	void registerEvents();
	void publishPropulsionOdometer(const IPCDeviceProxyEventEQEP& eqep);

	std::shared_ptr<IPCDeviceProxyService> m_pru0IpcProxyService;
	std::unique_ptr<MotorDriver8833> m_motorDriver;
};

#endif /* MOTORREGULATORSERVICE_H_ */
