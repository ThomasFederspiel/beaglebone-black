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
class IPCDeviceEQepProxy;
class MotorSKU415;
class MotorServiceCUIMessage;
class MotorDriver8833;
class MotorPIDRegulator;
class MotorRawRegulator;
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
	void handleCUIMessage(const MotorServiceCUIMessage& message);
	void handleSetRegulatorMode(const MotorServiceCUIMessage& message);
	void handleSetMotorSpeed(const MotorServiceCUIMessage& message);
	void handleSetMotorDistance(const MotorServiceCUIMessage& message);
	void handleGetMotorStatus(const MotorServiceCUIMessage& message);

	void applySpeedPIDRegulator(const MotorServiceCUIMessage& message);
	void applySpeedRawRegulator(const MotorServiceCUIMessage& message);
	void registerEvents();
	void publishPropulsionOdometer(const IPCDeviceProxyEventEQEP& eqep);
	void updateRegulators(const IPCDeviceProxyEventEQEP& eqep);
	void update(const IPCDeviceProxyEventEQEP& eqep);

	std::shared_ptr<IPCDeviceProxyService> m_ipcProxyService;
	std::unique_ptr<MotorDriver8833> m_motorDriver;
	std::unique_ptr<MotorSKU415> m_motorLeftDriver;
	std::unique_ptr<MotorSKU415> m_motorRightDriver;
	std::unique_ptr<MotorPIDRegulator> m_motorPidRegulator;
	std::unique_ptr<MotorRawRegulator> m_motorRawRegulator;
	std::unique_ptr<IPCDeviceEQepProxy> m_rightMotorEQepProxy;
	std::unique_ptr<IPCDeviceEQepProxy> m_leftMotorEQepProxy;
};

#endif /* MOTORREGULATORSERVICE_H_ */
