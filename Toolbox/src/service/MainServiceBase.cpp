/*
 * MainServiceBase.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "MainServiceBase.h"

// standard
#include <exception>

// local
#include "AsyncTaskMessage.h"
#include "ServiceMessageBase.h"
#include "StopServiceMessage.h"
#include "SystemMessage.h"
#include "SystemMessageTypes.h"

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "ShutdownMessage.h"
#include "tboxdefs.h"

MODULE_LOG(MainServiceBase);

MainServiceBase::MainServiceBase() : m_terminate(false), m_queue()
{
}

void MainServiceBase::shutdown()
{
	m_queue.clear();

	post(ShutdownMessage());
}

void MainServiceBase::post(const ServiceMessageBase& message)
{
	m_queue.put(message.clone());
}

void MainServiceBase::onMessage(ServiceMessageBase& message)
{
	SystemMessage* systemMessage = nullptr;
	const bool isInstance = tbox::instanceof<SystemMessage>(message, systemMessage);
	TB_ASSERT(isInstance);

	switch (systemMessage->getType())
	{
	case SystemMessageTypes::Type::ShutdownMessage:
	{
		const ShutdownMessage* shutdownMessage = nullptr;
		const bool isInstance = tbox::instanceof<const ShutdownMessage>(message, shutdownMessage);
		TB_ASSERT(isInstance);

		m_terminate = true;
	}
	break;

	case SystemMessageTypes::Type::AsyncTaskMessage:
	{
		onAsyncTaskMessage(message);
	}
	break;

	TB_DEFAULT(SystemMessageTypes::toString(systemMessage->getType()));
	}
}

void MainServiceBase::onAsyncTaskMessage(ServiceMessageBase& message)
{
	AsyncTaskMessage* asyncTaskMessage = nullptr;
	const bool isInstance = tbox::instanceof<AsyncTaskMessage>(message, asyncTaskMessage);
	TB_ASSERT(isInstance);

	asyncTaskMessage->operator()();
}

void MainServiceBase::run()
{
	try
	{
		onStart();

		while (!m_terminate)
		{
			const std::unique_ptr<ServiceMessageBase> message = m_queue.get();

			onMessage(*message);
		}

		onStop();
	}
	catch (std::exception& e)
	{
		ERROR("Exception killed main service " << e.what());

		throw e;
	}
	catch (...)
	{
		ERROR("Unknown exception killed main service");

		throw std::runtime_error("Main service died");
	}
}


