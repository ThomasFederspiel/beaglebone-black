/*
 * IPCServiceBase.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_SERVICEBASE_H_
#define PRUIPC_SERVICEBASE_H_

// project
#include "AbstractService.h"
#include "IPCMessageBase.h"
#include "ServiceMessageBase.h"

namespace pruipcservice
{

class IPCServiceBase : public AbstractService
{
public:

	virtual void onMessage(const IPCMessageBase& message) = 0;

protected:

	explicit IPCServiceBase(const std::string&);

	void onMessage(const ServiceMessageBase& message) override;

private:

};

} // namespace

#endif /* PRUIPC_SERVICEBASE_H_ */
