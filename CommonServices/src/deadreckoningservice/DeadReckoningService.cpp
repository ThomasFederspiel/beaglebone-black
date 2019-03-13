/*
 * DeadReckoningService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "DeadReckoningService.h"

// standard

// project
#include "CommonMessageTypes.h"
#include "DeadReckoningAlgoBasic.h"
#include "DeadReckoningMessage.h"
#include "exceptionMacros.h"
#include "IPCDeviceProxyEventOdo.h"
#include "IPCDeviceProxyService.h"
#include "IPCMessageTypes.h"
#include "Logger.h"
#include "ServiceAllocator.h"
#include "ServiceMessageBase.h"
#include "stdExtension.h"
#include "TMath.h"

// local

MODULE_LOG(DeadReckoningService);

using namespace commonservices;

namespace commonservices
{
extern const char* Pru0ProxyService;
};

namespace
{
static constexpr float WheelBase_m = 0.10f;
static constexpr float WheelRadius_m = 0.03f;
static constexpr float WheelCircumreference_m = 2 * tbox::Math::pi() *  WheelRadius_m;

// Measured to be 1500 rev per revolusion
static constexpr float MotorQuadraturePulsesPerRev = 1500.f;

static constexpr float WheelCircumreference_per_motorQuadraturePulse = WheelCircumreference_m / MotorQuadraturePulsesPerRev;

static constexpr PwmssDeviceEnum RightMotorEQepDevice = PWMSS_DEV_2;
static constexpr PwmssDeviceEnum LeftMotorEQepDevice = PWMSS_DEV_3;
};

DeadReckoningService::DeadReckoningService(const std::string& name) : AbstractService(name),
	m_ipcProxyService(), m_deadReckoningAlgo(), m_deltaInitiated(false), m_lastRightOdo(0.f), m_lastLeftOdo(0.f)
{
	m_deadReckoningAlgo = std::make_unique<DeadReckoningAlgoBasic>(WheelBase_m);

	registerEvents();
}

DeadReckoningService::~DeadReckoningService()
{
	// needed for unique_ptr
}

void DeadReckoningService::registerEvents()
{
	registerEvent(commonservices::CommonMessageTypes::Type::DeadReckoningMessage);
}

void DeadReckoningService::onStart(ServiceAllocator& allocator)
{
	m_ipcProxyService = allocator.allocateService<IPCDeviceProxyService>(commonservices::Pru0ProxyService, *this);
	TB_ASSERT(m_ipcProxyService);

	m_ipcProxyService->subscribeEvent(pruipcservice::IPCMessageTypes::IPCDeviceProxyEventOdo, *this);
}

DeadReckoningService::StopStatus DeadReckoningService::onStop(ServiceAllocator& allocator)
{
	m_ipcProxyService->unsubscribeAllEvents(*this);

	allocator.releaseService(m_ipcProxyService, *this);

	return StopStatus::Done;
}

// ;+
#include <chrono>

static auto last = std::chrono::steady_clock::now();

void DeadReckoningService::onMessage(const ServiceMessageBase& message)
{
	switch (message.getType())
	{
	case pruipcservice::IPCMessageTypes::IPCDeviceProxyEventOdo:
	{
		auto& odo = message.getCasted<IPCDeviceProxyEventOdo>();

		update(odo);

//		INFO("PruTime = " << odo.getPruTime());
//		INFO("Counter 1 = " << odo.getCounter_1());
//		INFO("Counter 2 = " << odo.getCounter_2());
	}
	break;

 	TB_DEFAULT(CommonMessageTypes::toString(message.getType()));
	}
}

void DeadReckoningService::update(const IPCDeviceProxyEventOdo& odo)
{
	float leftOdoDelta_m = 0;
	float rightOdoDelta_m = 0;

	if (calculateDeltaOdo(odo, leftOdoDelta_m, rightOdoDelta_m))
	{
		TB_ASSERT(m_deadReckoningAlgo);
		m_deadReckoningAlgo->update(leftOdoDelta_m, rightOdoDelta_m);

		publishDeadReckoning();
	}
}

bool DeadReckoningService::calculateDeltaOdo(const IPCDeviceProxyEventOdo& odo,
	float& leftOdoDelta_m, float& rightOdoDelta_m)
{
	if (odo.getPwmssDevice_1() == RightMotorEQepDevice)
	{
		rightOdoDelta_m = odo.getCounter_1();
		leftOdoDelta_m = odo.getCounter_2();
	}
	else
	{
		rightOdoDelta_m = odo.getCounter_2();
		leftOdoDelta_m = odo.getCounter_1();
	}

	if (m_deltaInitiated)
	{
		auto rightOdoCpy = rightOdoDelta_m;
		auto leftOdoCpy = leftOdoDelta_m;

		rightOdoDelta_m = (rightOdoDelta_m - m_lastRightOdo) * WheelCircumreference_per_motorQuadraturePulse;
		leftOdoDelta_m = (leftOdoDelta_m - m_lastLeftOdo) * WheelCircumreference_per_motorQuadraturePulse;

		m_lastRightOdo = rightOdoCpy;
		m_lastLeftOdo = leftOdoCpy;
	}
	else
	{
		m_lastRightOdo = rightOdoDelta_m;
		m_lastLeftOdo = leftOdoDelta_m;

		rightOdoDelta_m = 0.f;
		leftOdoDelta_m = 0.f;

		m_deltaInitiated = true;

		return false;
	}

	return true;
}

void DeadReckoningService::publishDeadReckoning()
{
	TB_ASSERT(m_deadReckoningAlgo);
	const DeadReckoningMessage message(m_deadReckoningAlgo->getPose());

	publishEvent(message);
}
