/*
 * ServiceMessageBase.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_SERVICEMESSAGEBASE_H_
#define TBOX_SERVICEMESSAGEBASE_H_

// standard
#include <chrono>
#include <memory>

// local
#include "ServiceTypes.h"

// project
#include "exceptionMacros.h"
#include "tboxdefs.h"

class ServiceMessageBase
{
public:
	friend class SystemMessage;
	friend class AbstractService;

	virtual ~ServiceMessageBase() = default;

	using properties_t = uint16_t;

	MessageType_t getType() const
	{
		return m_messageType;
	}

	bool isPeriodic() const
	{
		return m_properties & PeriodicTransientMask;
	}

	template<typename T>
	const T& getCasted() const
	{
		const T* castedMsg = nullptr;
		const bool isInstance = tbox::instanceof<const T>(*this, castedMsg);
		TB_ASSERT(isInstance);

		return *castedMsg;
	}

	virtual std::unique_ptr<ServiceMessageBase> clone() const = 0;

private:

	enum class Classification
	{
		System = 0,
		Client
	};

protected:

	enum Properties
	{
		TransientMessage = 0x00,
		PeriodicMessage = 0x01
	};

	ServiceMessageBase(const MessageType_t messageType) : ServiceMessageBase(messageType, TransientMessage)
	{
	}

	ServiceMessageBase(const MessageType_t messageType, const properties_t properties) : m_messageType(messageType), m_classification(Classification::Client),
		m_properties(properties)
	{
	}

	void shutdown();

private:

	enum ProperitesMask
	{
		PeriodicTransientMask = 0x01
	};

	ServiceMessageBase(const MessageType_t messageType, const Classification classification) : m_messageType(messageType), m_classification(classification),
		m_postTimePoint()
	{
	}

	bool isSystemMessage() const
	{
		return m_classification == Classification::System;
	}

	Classification getClassification() const
	{
		return m_classification;
	}

	void setPostTimePoint()
	{
		m_postTimePoint = std::chrono::steady_clock::now();
	}

	std::chrono::steady_clock::time_point getPostTimePoint()
	{
		return m_postTimePoint;
	}

	MessageType_t m_messageType;
	Classification m_classification;

	properties_t m_properties;

	std::chrono::steady_clock::time_point m_postTimePoint;
};

#endif /* TBOX_SERVICEMESSAGEBASE_H_ */
