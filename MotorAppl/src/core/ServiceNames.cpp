/*
 * ServiceNames.cpp
 *
 *  Created on: 20 nov 2016
 *      Author: Thomas
 */

#include "ServiceNames.h"

const char* ServiceNames::SoundService = "SoundService";
const char* ServiceNames::Pru0ProxyService = "Pru0ProxyService";
const char* ServiceNames::Pru1ProxyService = "Pru1ProxyService";
const char* ServiceNames::PruManagerService = "PruManagerService";
const char* ServiceNames::MotorRegulatorService = "MotorRegulatorService";
const char* ServiceNames::TelnetService = "TelnetService";
const char* ServiceNames::StreamService = "StreamService";
const char* ServiceNames::DeadReckoningService = "DeadReckoningService";

namespace commonservices
{
const char* Pru0ProxyService = ServiceNames::Pru0ProxyService;
const char* Pru1ProxyService = ServiceNames::Pru1ProxyService;
};

