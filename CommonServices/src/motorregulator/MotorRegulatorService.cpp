/*
 * MotorRegulatorService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "MotorRegulatorService.h"

// standard
#include <cstdint>

// project
#include "exceptionMacros.h"
#include "CommonMessageTypes.h"
#include "CUICommands.h"
#include "IPCDeviceGpioProxy.h"
#include "IPCDeviceEQepProxy.h"
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

namespace
{
static constexpr PwmssDeviceEnum RightMotorEQepDevice = PWMSS_DEV_2;
static constexpr PwmssDeviceEnum LeftMotorEQepDevice = PWMSS_DEV_3;

static constexpr IPCDeviceGpioProxy::GpioPins SleepPin = IPCDeviceGpioProxy::GpioPins::PRU0_GPIO_xx_P9_41_OUT;
static constexpr IPCDeviceGpioProxy::GpioPins FaultPin = IPCDeviceGpioProxy::GpioPins::PRU0_GPIO_xx_P9_42_IN;

static constexpr float LeftMotorMaxRPM = 100.f;
static constexpr float RightMotorMaxRPM = 100.f;
static constexpr float LeftMotorQuadraturePulses = 341.2f;
static constexpr float RightMotorQuadraturePulses = 341.2f;
}

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
		m_motorDriver(), m_rightMotorEQepProxy(), m_leftMotorEQepProxy()
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

	m_rightMotorEQepProxy = tbox::make_unique<IPCDeviceEQepProxy>(*m_pru0IpcProxyService, RightMotorEQepDevice);
	m_leftMotorEQepProxy = tbox::make_unique<IPCDeviceEQepProxy>(*m_pru0IpcProxyService, LeftMotorEQepDevice);

	m_rightMotorEQepProxy->enableEQepQuadrature(EQEP_UNIT_TIMER_PERIOD_100ms,
			EQEP_CAPCLK_DIV_16, EQEP_UPEVNT_DIV_4);

//	pulses_per_sec = rpm / 60 * pulses_per_rev
//	pulse_time_ms = 60000 / (rpm * pulses_per_rev)
//	upevent_time_ms = pulse_time_ms * upevent_div / 4
//	max_clk_time_ms = clk_div * 100 * 65535 / 1000000
//	upevent_time_ms < max_clk_time_ms

	m_leftMotorEQepProxy->enableEQepQuadrature(EQEP_UNIT_TIMER_PERIOD_100ms,
			EQEP_CAPCLK_DIV_16, EQEP_UPEVNT_DIV_4);

	m_motorDriver = tbox::make_unique<MotorDriver8833>(*m_pru0IpcProxyService, LeftMotorEQepDevice, RightMotorEQepDevice,
			SleepPin, FaultPin);
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

	m_rightMotorEQepProxy->disableEQep();
	m_rightMotorEQepProxy.reset();

	m_leftMotorEQepProxy->disableEQep();
	m_leftMotorEQepProxy.reset();

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
		publishPropulsionOdometer(message.getCasted<IPCDeviceProxyEventEQEP>());
		break;

 	TB_DEFAULT("Unhandled value " << CommonMessageTypes::toString(message.getType()));
	}
}

void MotorRegulatorService::applyMotion(const MotionMessage& message)
{
	switch (message.getMotor())
	{
	case MotionMessage::Motor::AllMotors:
// ;+		applyAction(message.getAction(), m_motorDriver->getLeftMotor(), message.getLeftSpeed());
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
	case RightMotorEQepDevice:
		motor = PropulsionOdometerMessage::Motor::RightMotor;
		break;

	case LeftMotorEQepDevice:
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

	// ;+
	//INFO("device = " << eqep.getPwmssDevice());
	//INFO("Counter = " << eqep.getCounter());

	publishEvent(message);
}
