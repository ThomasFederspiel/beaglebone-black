/*
 * AsyncTaskMessage.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "AsyncTaskMessage.h"

// standard
#include <memory>

// project
#include "Logger.h"
#include "ServiceMessageBase.h"
#include "stdExtension.h"

MODULE_LOG(AsyncTaskMessage);

std::unique_ptr<ServiceMessageBase> AsyncTaskMessage::clone() const
{
	return std::make_unique<AsyncTaskMessage>(*this);
}

void AsyncTaskMessage::operator()()
{
	m_task();
}




