/*
 * DeadReckoningService.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef MQTTSERVICE_H_
#define MQTTSERVICE_H_

// standard
#include <memory>

// project
#include "AbstractService.h"
#include "ServiceAllocator.h"

class IDeadReckoningAlgo;
class IPCDeviceProxyEventOdo;
class IPCDeviceProxyService;
class ProtocolManager;

class DeadReckoningService final : public AbstractService
{
public:

	explicit DeadReckoningService(const std::string& name);
	~DeadReckoningService();

protected:

	void onMessage(const ServiceMessageBase& message) override;
	void onStart(ServiceAllocator& allocator) override;
	StopStatus onStop(ServiceAllocator& allocator) override;

private:
	void update(const IPCDeviceProxyEventOdo& odo);
	bool calculateDeltaOdo(const IPCDeviceProxyEventOdo& odo, float& leftOdoDelta, float& rightOdoDelta);
	void publishDeadReckoning();
	void registerEvents();

	std::shared_ptr<IPCDeviceProxyService> m_ipcProxyService;
	std::unique_ptr<IDeadReckoningAlgo> m_deadReckoningAlgo;
	bool m_deltaInitiated;
	float m_lastRightOdo;
	float m_lastLeftOdo;
};

#endif /* MQTTSERVICE_H_ */
