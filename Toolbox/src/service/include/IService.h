/*
 * IService.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_ISERVICE_H_
#define TBOX_ISERVICE_H_

// standard
#include <string>

class ServiceAllocator;

class IService
{
public:

	IService() = default;
	virtual ~IService() = default;

    IService(const IService&) = delete;
    IService& operator=(const IService&) = delete;

	virtual const std::string& name() const = 0;
	virtual void start(ServiceAllocator& allocator) = 0;
	virtual void stop(ServiceAllocator& allocator) = 0;
};

#endif /* TBOX_ISERVICE_H_ */
