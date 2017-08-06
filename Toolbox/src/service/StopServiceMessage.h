/*
 * StopServiceMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_STOPSERVICMESSAGE_H_
#define TBOX_STOPSERVICMESSAGE_H_

// project
#include "ServiceAllocator.h"
#include "SystemMessage.h"
#include "SystemMessageTypes.h"
#include "tboxdefs.h"

class StopServiceMessage final : public SystemMessage
{
public:

	explicit StopServiceMessage(ServiceAllocator& allocator) : SystemMessage(SystemMessageTypes::Type::StopMessage), m_allocator(allocator)
	{
	}

	ServiceAllocator& getAllocator() const
	{
		return m_allocator;
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return tbox::make_unique<StopServiceMessage>(*this);
	}

private:

	mutable ServiceAllocator m_allocator;
};

#endif /* TBOX_STOPSERVICMESSAGE_H_ */
