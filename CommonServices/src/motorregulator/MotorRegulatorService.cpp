/*
 * MotorRegulatorService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "MotorRegulatorService.h"

// standard

// project
#include "exceptionMacros.h"
#include "CommonMessageTypes.h"
#include "CUICommands.h"
#include "IPCDeviceGpioProxy.h"
#include "IPCDeviceProxyService.h"
#include "IPCDeviceProxyEventEQEP.h"
#include "IPCMessageTypes.h"
#include "Logger.h"
#include "MotionMessage.h"
#include "MotorDriver8833.h"
#include "PropulsionOdometerMessage.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

// local
#include "PropulsionOdometerMessage.h"

MODULE_LOG(MotorRegulatorService);

using namespace commonservices;

namespace commonservices
{
extern const char* Pru0ProxyService;

static void applyAction(const MotionMessage::MotorAction action, MotorDriver8833::Motor& motor,
		const float speed)
{
	switch (action)
	{
	case MotionMessage::MotorAction::Run:
		motor.setSpeed(speed);
		break;

	case MotionMessage::MotorAction::Coast:
		motor.coast();
		break;

	case MotionMessage::MotorAction::Stop:
		motor.stop();
		break;

	TB_DEFAULT(MotionMessage::toString(action));
	}
}

};

MotorRegulatorService::MotorRegulatorService(const std::string& name) : AbstractService(name), m_pru0IpcProxyService(),
		m_motorDriver()
{
	registerEvents();
}

MotorRegulatorService::~MotorRegulatorService()
{
	// needed for unique_ptr
}

void MotorRegulatorService::registerEvents()
{
	registerEvent(commonservices::CommonMessageTypes::Type::LeftPropulsionOdometerMessage);
	registerEvent(commonservices::CommonMessageTypes::Type::RightPropulsionOdometerMessage);
}

void MotorRegulatorService::onStart(ServiceAllocator& allocator)
{
	m_pru0IpcProxyService = allocator.allocateService<IPCDeviceProxyService>(commonservices::Pru0ProxyService, *this);
	TB_ASSERT(m_pru0IpcProxyService);

	m_pru0IpcProxyService->subscribeEvent(pruipcservice::IPCMessageTypes::IpcDeviceProxyEventEQEP, *this);

	m_motorDriver = tbox::make_unique<MotorDriver8833>(*m_pru0IpcProxyService, PWMSS_DEV_2, PWMSS_DEV_1,
			IPCDeviceGpioProxy::GpioPins::PRU0_GPIO_xx_P9_41_OUT, IPCDeviceGpioProxy::GpioPins::PRU0_GPIO_xx_P9_42_IN);
	TB_ASSERT(m_motorDriver);

	m_motorDriver->getLeftMotor().open();
	m_motorDriver->getRightMotor().open();
	m_motorDriver->setSleepMode(false);

	constexpr bool reg = true;
	manageCUICommands(allocator, reg);
}

MotorRegulatorService::StopStatus MotorRegulatorService::onStop(ServiceAllocator& allocator)
{
	m_motorDriver->setSleepMode(true);
	m_motorDriver->getLeftMotor().close();
	m_motorDriver->getRightMotor().close();
	m_motorDriver.reset();

	m_pru0IpcProxyService->unsubscribeAllEvents(*this);

	allocator.releaseService(m_pru0IpcProxyService, *this);

	constexpr bool reg = false;
	manageCUICommands(allocator, reg);

	return StopStatus::Done;
}

void MotorRegulatorService::onMessage(const ServiceMessageBase& message)
{
	switch (message.getType())
	{
	case CommonMessageTypes::MotionMessage:
		applyMotion(message.getCasted<MotionMessage>());
		break;

	case pruipcservice::IPCMessageTypes::IpcDeviceProxyEventEQEP:
	{
		publishPropulsionOdometer(message.getCasted<IPCDeviceProxyEventEQEP>());
	}
	break;

 	TB_DEFAULT("Unhandled value " << CommonMessageTypes::toString(message.getType()));
	}
}

void MotorRegulatorService::applyMotion(const MotionMessage& message)
{
	switch (message.getMotor())
	{
	case MotionMessage::Motor::AllMotors:
		applyAction(message.getAction(), m_motorDriver->getLeftMotor(), message.getLeftSpeed());
		applyAction(message.getAction(), m_motorDriver->getRightMotor(), message.getRightSpeed());
		break;

	case MotionMessage::Motor::LeftMotor:
		applyAction(message.getAction(), m_motorDriver->getLeftMotor(), message.getLeftSpeed());
		break;

	case MotionMessage::Motor::RightMotor:
		applyAction(message.getAction(), m_motorDriver->getRightMotor(), message.getRightSpeed());
		break;

	TB_DEFAULT(message.toString(message.getMotor()));
	}
}

void MotorRegulatorService::manageCUICommands(ServiceAllocator& allocator, const bool reg)
{
	if (reg)
	{
		allocator.registerCommand(tbox::make_unique<SetMotorSpeedCommand>(*this));
	}
	else
	{
		allocator.unregisterCommand(SetMotorSpeedCommand(*this));
	}
}

void MotorRegulatorService::publishPropulsionOdometer(const IPCDeviceProxyEventEQEP& eqep)
{
	PropulsionOdometerMessage::Motor motor = PropulsionOdometerMessage::Motor::RightMotor;

	switch (eqep.getPwmssDevice())
	{
	case PWMSS_DEV_2:
		motor = PropulsionOdometerMessage::Motor::RightMotor;
		break;

	case PWMSS_DEV_3:
		motor = PropulsionOdometerMessage::Motor::LeftMotor;
		break;

	TB_DEFAULT(eqep.getPwmssDevice());
	}

	const PropulsionOdometerMessage message(motor,
			eqep.getCapCounter(),
			eqep.getCapTime(),
			eqep.getCapPeriod(),
			eqep.getCounter()
			);

	publishEvent(message);
}
