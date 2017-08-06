/*
 * MainServiceBase.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_MAINSERVICEBASE_H_
#define TBOX_MAINSERVICEBASE_H_

// standard
#include <memory>

// local

// project
#include "AtomicQueue.h"

class ServiceMessageBase;

class MainServiceBase
{
public:

	virtual ~MainServiceBase() = default;

	void run();

	// Thread safe
	void post(const ServiceMessageBase& message);

protected:

	MainServiceBase();

	void shutdown();

	virtual void onStart() = 0;
	virtual void onStop() = 0;

private:

	void onMessage(ServiceMessageBase& message);
	void onAsyncTaskMessage(ServiceMessageBase& message);

	bool m_terminate;
	tbox::AtomicQueue<std::unique_ptr<ServiceMessageBase>> m_queue;
};

#endif /* TBOX_MAINSERVICEBASE_H_ */
