/*
 *  IMessageService.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_IMESSAGESERVICE_H_
#define TBOX_IMESSAGESERVICE_H_

// local
#include "AbstractEventMessagePublisher.h"
#include "IMessageReceiver.h"
#include "IService.h"

class IMessageService : public IService, public IMessageReceiver, public AbstractEventMessagePublisher
{
};

#endif /* TBOX_IMESSAGESERVICE_H_ */
