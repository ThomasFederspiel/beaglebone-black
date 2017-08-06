/*
 * StartServiceMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_STARTSERVICMESSAGE_H_
#define TBOX_STARTSERVICMESSAGE_H_

// project
#include "ServiceAllocator.h"
#include "SystemMessage.h"
#include "SystemMessageTypes.h"
#include "tboxdefs.h"

class StartServiceMessage final : public SystemMessage
{
public:

	explicit StartServiceMessage(ServiceAllocator& allocator) : SystemMessage(SystemMessageTypes::Type::StartMessage), m_allocator(allocator)
	{
	}

	ServiceAllocator& getAllocator() const
	{
		return m_allocator;
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return tbox::make_unique<StartServiceMessage>(*this);
	}

private:

	mutable ServiceAllocator m_allocator;
};

#endif /* TBOX_STARTSERVICMESSAGE_H_ */
