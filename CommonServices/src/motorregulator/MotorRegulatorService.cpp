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
#include "IPCDeviceProxyEventTrace.h"
#include "IPCMessageTypes.h"
#include "Logger.h"
#include "MotorServiceCUIMessage.h"
#include "MotorDriver8833.h"
#include "MotorPIDRegulator.h"
#include "MotorRawRegulator.h"
#include "PropulsionOdometerMessage.h"
#include "PropulsionPidMessage.h"
#include "ServiceAllocator.h"
#include "ServiceMessageBase.h"
#include "SpeedEvaluator.h"
#include "stdExtension.h"
#include "Stringifyer.h"
#include "Utils.h"

// local
#include "MotorSKU415.h"
#include "PropulsionOdometerMessage.h"

MODULE_LOG(MotorRegulatorService);

using namespace commonservices;

namespace
{
static constexpr PwmssDeviceEnum RightMotorEQepDevice = PWMSS_DEV_2;
static constexpr PwmssDeviceEnum RightMotorPwmDevice = PWMSS_DEV_2;

static constexpr PwmssDeviceEnum LeftMotorEQepDevice = PWMSS_DEV_3;
static constexpr PwmssDeviceEnum LeftMotorPwmDevice = PWMSS_DEV_1;

static constexpr IPCDeviceGpioProxy::GpioPins SleepPin = IPCDeviceGpioProxy::GpioPins::PRU0_GPIO_xx_P9_41_OUT;
static constexpr IPCDeviceGpioProxy::GpioPins FaultPin = IPCDeviceGpioProxy::GpioPins::PRU0_GPIO_xx_P9_42_IN;
}

namespace commonservices
{
extern const char* Pru0ProxyService;
};

MotorRegulatorService::MotorRegulatorService(const std::string& name) : AbstractService(name), m_ipcProxyService(),
		m_motorDriver(), m_motorLeftDriver(), m_motorRightDriver(), m_motorPidRegulator(), m_motorRawRegulator(),
		m_rightMotorEQepProxy(), m_leftMotorEQepProxy()
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
	registerEvent(commonservices::CommonMessageTypes::Type::LeftPropulsionPidMessage);
	registerEvent(commonservices::CommonMessageTypes::Type::RightPropulsionPidMessage);
}

void MotorRegulatorService::onStart(ServiceAllocator& allocator)
{
	m_ipcProxyService = allocator.allocateService<IPCDeviceProxyService>(commonservices::Pru0ProxyService, *this);
	TB_ASSERT(m_ipcProxyService);

	m_ipcProxyService->subscribeEvent(pruipcservice::IPCMessageTypes::IPCDeviceProxyEventEQEP, *this);
	m_ipcProxyService->subscribeEvent(pruipcservice::IPCMessageTypes::IPCDeviceProxyEventTrace, *this);

	m_rightMotorEQepProxy = std::make_unique<IPCDeviceEQepProxy>(*m_ipcProxyService, RightMotorEQepDevice);
	m_leftMotorEQepProxy = std::make_unique<IPCDeviceEQepProxy>(*m_ipcProxyService, LeftMotorEQepDevice);

	m_rightMotorEQepProxy->open();
	m_rightMotorEQepProxy->enableEQepQuadrature(EQEP_UNIT_TIMER_PERIOD_10ms,
			EQEP_CAPCLK_DIV_16, EQEP_UPEVNT_DIV_4, EQEP_QUADRATURE_DEFAULT_MODE);

	m_leftMotorEQepProxy->open();
	m_leftMotorEQepProxy->enableEQepQuadrature(EQEP_UNIT_TIMER_PERIOD_10ms,
			EQEP_CAPCLK_DIV_16, EQEP_UPEVNT_DIV_4, EQEP_QUADRATURE_SWAP_MODE);

	m_motorDriver = std::make_unique<MotorDriver8833>(*m_ipcProxyService, LeftMotorPwmDevice, RightMotorPwmDevice,
			SleepPin, FaultPin);
	TB_ASSERT(m_motorDriver);

	m_motorDriver->open();
	m_motorDriver->setSleepMode(false);

	const MotorSKU415::SKU415Config leftMotorConfig(30.8, 178.7, 8.831, 2421.776);
	m_motorLeftDriver = std::make_unique<MotorSKU415>(m_motorDriver->getLeftMotor(), leftMotorConfig);

	const MotorSKU415::SKU415Config rightMotorConfig(21.6, 188.7, 9.007, 2299.779);
	m_motorRightDriver = std::make_unique<MotorSKU415>(m_motorDriver->getRightMotor(), rightMotorConfig);

	m_motorPidRegulator = std::make_unique<MotorPIDRegulator>(*m_motorLeftDriver, *m_motorRightDriver);
	m_motorRawRegulator = std::make_unique<MotorRawRegulator>(*m_motorLeftDriver, *m_motorRightDriver);

	// ;+
	m_motorRawRegulator->setMode(MotorRawRegulator::Mode::RPM);

	motorregulator::CUICommands::registerCUICommands(allocator, *this);
}

MotorRegulatorService::StopStatus MotorRegulatorService::onStop(ServiceAllocator& allocator)
{
	m_motorPidRegulator.reset();
	m_motorRawRegulator.reset();

	m_motorDriver->setSleepMode(true);
	m_motorDriver->close();
	m_motorDriver.reset();

	m_rightMotorEQepProxy->disableEQep();
	m_rightMotorEQepProxy->close();
	m_rightMotorEQepProxy.reset();

	m_leftMotorEQepProxy->disableEQep();
	m_leftMotorEQepProxy->close();
	m_leftMotorEQepProxy.reset();

	m_ipcProxyService->unsubscribeAllEvents(*this);

	allocator.releaseService(m_ipcProxyService, *this);

	motorregulator::CUICommands::unregisterCUICommands(allocator, *this);

	return StopStatus::Done;
}

void MotorRegulatorService::onMessage(const ServiceMessageBase& message)
{
	switch (message.getType())
	{
	case CommonMessageTypes::MotorServiceCUIMessage:
		handleCUIMessage(message.getCasted<MotorServiceCUIMessage>());
		break;

	case pruipcservice::IPCMessageTypes::IPCDeviceProxyEventTrace:
	{
		auto& trace = message.getCasted<IPCDeviceProxyEventTrace>();

		auto traceData = trace.getTrace();
		auto size = trace.size();

	    const std::vector<std::string> labels =
		{
			"Device",
			"QCAPCTL",
			"QUPRD",
			"QFLG",
			"QEINT",
			"QEPCTL"
		};

		for (std::size_t i = 0; i < size; ++i)
		{
			std::string label = TBOX_STRINGIFY(i);

			if (i < labels.size())
			{
				label = labels.at(i);
			}

 			INFO("trace[" << label << "] = " << traceData[i] << ", 0x" << std::hex << traceData[i]
			 	 << "("<< Utils::toBinaryStr(traceData[i]) << ")");
		}
	}
	break;

	case pruipcservice::IPCMessageTypes::IPCDeviceProxyEventEQEP:
	{
		auto& eqep = message.getCasted<IPCDeviceProxyEventEQEP>();

		update(eqep);

		publishPropulsionOdometer(eqep);
		publishPropulsionPid();
	}
	break;

 	TB_DEFAULT(CommonMessageTypes::toString(message.getType()));
	}
}

void MotorRegulatorService::update(const IPCDeviceProxyEventEQEP& eqep)
{
	switch (eqep.getPwmssDevice())
	{
	case RightMotorEQepDevice:
		m_motorRightDriver->update(eqep);
		m_motorPidRegulator->updateRight();
		break;

	case LeftMotorEQepDevice:
		m_motorLeftDriver->update(eqep);
		m_motorPidRegulator->updateLeft();
		break;

	TB_DEFAULT(eqep.getPwmssDevice());
	}
}

void MotorRegulatorService::handleCUIMessage(const MotorServiceCUIMessage& message)
{
	switch (message.getMessageType())
	{
	case MotorServiceCUIMessage::CUIMessageType::SetRegulatorMode:
		handleSetRegulatorMode(message);
		break;

	case MotorServiceCUIMessage::CUIMessageType::setMotorSpeed:
		handleSetMotorSpeed(message);
		break;

	case MotorServiceCUIMessage::CUIMessageType::setMotorDistance:
		handleSetMotorDistance(message);
		break;

	case MotorServiceCUIMessage::CUIMessageType::SetMotorPidTuning:
		handleSetMotorPidTuning(message);
		break;

	case MotorServiceCUIMessage::CUIMessageType::GetMotorStatus:
		handleGetMotorStatus(message);
		break;

 	TB_DEFAULT(MotorServiceCUIMessage::toString(message.getMessageType()));
	}
}

void MotorRegulatorService::handleSetMotorPidTuning(const MotorServiceCUIMessage& message)
{
	switch (message.getMotor())
	{
	case MotorServiceCUIMessage::Motor::AllMotors:
		m_motorPidRegulator->setLeftTuning(message.getKpFactor(), message.getKiFactor(),
				message.getKdFactor());
		m_motorPidRegulator->setRightTuning(message.getKpFactor(), message.getKiFactor(),
				message.getKdFactor());
		break;

	case MotorServiceCUIMessage::Motor::LeftMotor:
		m_motorPidRegulator->setLeftTuning(message.getKpFactor(), message.getKiFactor(),
				message.getKdFactor());
		break;

	case MotorServiceCUIMessage::Motor::RightMotor:
		m_motorPidRegulator->setRightTuning(message.getKpFactor(), message.getKiFactor(),
				message.getKdFactor());
		break;

	TB_DEFAULT(message.toString(message.getMotor()));
	}
}

void MotorRegulatorService::handleSetMotorDistance(const MotorServiceCUIMessage& message)
{
	m_motorPidRegulator->enable(false);

	m_motorRawRegulator->setLeftDistance(message.getLeftDistance());
	m_motorRawRegulator->setRightDistance(message.getRightDistance());
}

void MotorRegulatorService::handleGetMotorStatus(const MotorServiceCUIMessage& message)
{
	auto& context = message.getCUIContext();

	context.output() << "Left motor ------" << context.newline();
	context.output() << "RPM = " << m_motorLeftDriver->getSpeedRPM() << context.newline();
	context.output() << "Counter = " << m_motorLeftDriver->getCounter() << context.newline();
	context.output() << "Right motor ------" << context.newline();
	context.output() << "RPM = " << m_motorRightDriver->getSpeedRPM() << context.newline();
	context.output() << "Counter = " << m_motorRightDriver->getCounter() << context.newline();
	context.finalize();
}

void MotorRegulatorService::handleSetMotorSpeed(const MotorServiceCUIMessage& message)
{
	if (m_motorPidRegulator->isEnabled())
	{
		applySpeedPIDRegulator(message);
	}
	else if (m_motorRawRegulator->isEnabled())
	{
		applySpeedRawRegulator(message);
	}
	else
	{
		ERROR("Speed not set as no regulator is active");
	}
}

void MotorRegulatorService::handleSetRegulatorMode(const MotorServiceCUIMessage& message)
{
	const auto mode = message.getRegulatorMode();

	switch (mode)
	{
	case MotorServiceCUIMessage::RegulatorMode::PidRegulation:
		m_motorPidRegulator->enable(true);
		m_motorRawRegulator->setMode(MotorRawRegulator::Mode::Disabled);
		break;

	case MotorServiceCUIMessage::RegulatorMode::Raw:
		m_motorPidRegulator->enable(false);
		m_motorRawRegulator->setMode(MotorRawRegulator::Mode::Raw);
		break;

	case MotorServiceCUIMessage::RegulatorMode::RPM:
		m_motorPidRegulator->enable(false);
		m_motorRawRegulator->setMode(MotorRawRegulator::Mode::RPM);
		break;

	case MotorServiceCUIMessage::RegulatorMode::Percentage:
		m_motorPidRegulator->enable(false);
		m_motorRawRegulator->setMode(MotorRawRegulator::Mode::Percentage);
		break;

	case MotorServiceCUIMessage::RegulatorMode::PercentageAdjusted:
		m_motorPidRegulator->enable(false);
		m_motorRawRegulator->setMode(MotorRawRegulator::Mode::PercentageAdjusted);
		break;

	TB_DEFAULT(MotorServiceCUIMessage::toString(mode));
	}
}

void MotorRegulatorService::applySpeedPIDRegulator(const MotorServiceCUIMessage& message)
{
	switch (message.getMotor())
	{
	case MotorServiceCUIMessage::Motor::AllMotors:
		m_motorPidRegulator->setLeftSpeed(message.getLeftSpeed());
		m_motorPidRegulator->setRightSpeed(message.getRightSpeed());
		break;

	case MotorServiceCUIMessage::Motor::LeftMotor:
		m_motorPidRegulator->setLeftSpeed(message.getLeftSpeed());
		break;

	case MotorServiceCUIMessage::Motor::RightMotor:
		m_motorPidRegulator->setRightSpeed(message.getRightSpeed());
		break;

	TB_DEFAULT(message.toString(message.getMotor()));
	}
}

void MotorRegulatorService::applySpeedRawRegulator(const MotorServiceCUIMessage& message)
{
	switch (message.getMotor())
	{
	case MotorServiceCUIMessage::Motor::AllMotors:
		m_motorRawRegulator->setLeftSpeed(message.getLeftSpeed());
		m_motorRawRegulator->setRightSpeed(message.getRightSpeed());
		break;

	case MotorServiceCUIMessage::Motor::LeftMotor:
		m_motorRawRegulator->setLeftSpeed(message.getLeftSpeed());
		break;

	case MotorServiceCUIMessage::Motor::RightMotor:
		m_motorRawRegulator->setRightSpeed(message.getRightSpeed());
		break;

	TB_DEFAULT(message.toString(message.getMotor()));
	}
}

void MotorRegulatorService::publishPropulsionPid()
{
	const auto leftState = m_motorPidRegulator->getLeftPidState();

	// ;+
//	INFO("leftState.m_setPoint = " << leftState.m_setPoint)
//	INFO("leftState.m_kpFactor = " << leftState.m_kpFactor)

	const PropulsionPidMessage leftMessage(PropulsionPidMessage::Motor::LeftMotor,
			leftState.m_setPoint,
			leftState.m_kpFactor,
			leftState.m_kiFactor,
			leftState.m_kdFactor,
			leftState.m_lastInput,
			leftState.m_lastError,
			leftState.m_lastOutput
			);

	publishEvent(leftMessage);

	const auto rightState = m_motorPidRegulator->getRightPidState();

	// ;+
//	INFO("rightState.m_setPoint = " << rightState.m_setPoint)
//	INFO("rightState.m_kpFactor = " << rightState.m_kpFactor)

	const PropulsionPidMessage rightMessage(PropulsionPidMessage::Motor::RightMotor,
			rightState.m_setPoint,
			rightState.m_kpFactor,
			rightState.m_kiFactor,
			rightState.m_kdFactor,
			rightState.m_lastInput,
			rightState.m_lastError,
			rightState.m_lastOutput
			);

	publishEvent(rightMessage);
}

void MotorRegulatorService::publishPropulsionOdometer(const IPCDeviceProxyEventEQEP& eqep)
{
	PropulsionOdometerMessage::Motor motor = PropulsionOdometerMessage::Motor::RightMotor;
	float speedRPM = 0.f;
	float lowSpeedRPM = 0.f;
	float highSpeedRPM = 0.f;
	bool lowSpeedValid = false;
	bool lowSpeedActive = false;

	switch (eqep.getPwmssDevice())
	{
	case RightMotorEQepDevice:
		motor = PropulsionOdometerMessage::Motor::RightMotor;
		speedRPM = m_motorRightDriver->getSpeedRPM();
		lowSpeedRPM = m_motorRightDriver->getLowSpeedRPM();
		lowSpeedValid = m_motorRightDriver->isLowSpeedValid();
		lowSpeedActive = m_motorRightDriver->isLowSpeedActive();
		highSpeedRPM = m_motorRightDriver->getHighSpeedRPM();
		break;

	case LeftMotorEQepDevice:
		motor = PropulsionOdometerMessage::Motor::LeftMotor;
		speedRPM = m_motorLeftDriver->getSpeedRPM();
		lowSpeedRPM = m_motorLeftDriver->getLowSpeedRPM();
		lowSpeedValid = m_motorLeftDriver->isLowSpeedValid();
		lowSpeedActive = m_motorLeftDriver->isLowSpeedActive();
		highSpeedRPM = m_motorLeftDriver->getHighSpeedRPM();
		break;
	TB_DEFAULT(eqep.getPwmssDevice());
	}

	const PropulsionOdometerMessage message(motor,
			eqep.getCapCounter(),
			eqep.getCapTime(),
			eqep.getCapPeriod(),
			eqep.getCounter(),
			speedRPM,
			highSpeedRPM,
			lowSpeedRPM,
			lowSpeedValid,
			lowSpeedActive
			);

	publishEvent(message);
}
