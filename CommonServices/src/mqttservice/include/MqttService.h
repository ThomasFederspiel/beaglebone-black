/*
 * MqttService.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef MQTTSERVICE_H_
#define MQTTSERVICE_H_

// standard

// project
#include "AbstractService.h"
#include "ServiceAllocator.h"

class ProtocolManager;

class MqttService final : public AbstractService
{
public:

	explicit MqttService(const std::string& name);
	~MqttService();

protected:

	void onMessage(const ServiceMessageBase& message) override;
	void onStart(ServiceAllocator& allocator) override;
	StopStatus onStop(ServiceAllocator& allocator) override;

private:

};

#endif /* MQTTSERVICE_H_ */
