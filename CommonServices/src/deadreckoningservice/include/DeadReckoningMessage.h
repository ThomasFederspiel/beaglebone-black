/*
 * DeadReckoningMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef DEADRECKONINGMESSAGE_H_
#define DEADRECKONINGMESSAGE_H_

// standard
#include <cstdint>

// project
#include "CommonMessageTypes.h"
#include "DeadReckoningPose.h"
#include "ServiceMessageBase.h"
#include "stdExtension.h"

class DeadReckoningMessage final : public ServiceMessageBase
{
public:

	explicit DeadReckoningMessage(const DeadReckoningPose& pose) : ServiceMessageBase(commonservices::CommonMessageTypes::Type::DeadReckoningMessage,
			ServiceMessageBase::PeriodicMessage), m_drPose(pose)
	{
	}

	const DeadReckoningPose& getPose() const
	{
		return m_drPose;
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return std::make_unique<DeadReckoningMessage>(*this);
	}
private:

	DeadReckoningPose m_drPose;
};

#endif /* DEADRECKONINGMESSAGE_H_ */
