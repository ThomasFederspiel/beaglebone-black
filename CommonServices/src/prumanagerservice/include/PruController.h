/*
 * PruController.h
 *
 *  Created on: 4 jan 2016
 *      Author: Thomas
 */

#ifndef PRUCONTROLLER_H_
#define PRUCONTROLLER_H_

// standard
#include <memory>

// project
#include "PrussDriver.h"
#include "PruImageConfig.h"

class PruController final
{
public:
	explicit PruController(const PruImageConfig& pruConfig);

	void setup();
	void start();
	void terminate();
	void close(PrussDriver::BiDirectionPruEventChannelPtr eventChannel);

private:

	PruImageConfig m_pruConfig;
	std::shared_ptr<IMemory> m_ddrRam;
	std::shared_ptr<IMemory> m_shram;
};

#endif /* PRUCONTROLLER_H_ */
