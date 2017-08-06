/*
 * ServiceMessageBase.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_SERVICEMESSAGEBASE_H_
#define TBOX_SERVICEMESSAGEBASE_H_

// standard
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

	MessageType_t getType() const
	{
		return m_messageType;
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

	ServiceMessageBase(const MessageType_t messageType) : m_messageType(messageType), m_classification(Classification::Client)
	{
	}

	void shutdown();

private:

	ServiceMessageBase(const MessageType_t messageType, const Classification classification) : m_messageType(messageType), m_classification(classification)
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

	MessageType_t m_messageType;
	Classification m_classification;
};

#endif /* TBOX_SERVICEMESSAGEBASE_H_ */
