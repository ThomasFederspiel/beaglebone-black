/*
 * TelnetService.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TELNETSERVICE_H_
#define TELNETSERVICE_H_

// standard
#include <memory>
#include <string>

// project
#include "AbstractService.h"
#include "ICUIManager.h"
#include "ServiceAllocator.h"

class CommandExecutor;
class ServiceMessageBase;
class TelnetServer;

class TelnetService final : public AbstractService, public ICUIManager
{
public:

	explicit TelnetService(const std::string& name);

	~TelnetService();

	/// Implementing ICUIManager
	void registerCommand(std::unique_ptr<ICUICommand> command) override;
	void unregisterCommand(const ICUICommand& command) override;

protected:

	void onMessage(const ServiceMessageBase& message) override;
	void onStart(ServiceAllocator& allocator) override;
	StopStatus onStop(ServiceAllocator& allocator) override;

private:
	std::unique_ptr<TelnetServer> m_telnetServer;
	std::unique_ptr<CommandExecutor> m_commandExecutor;
};

#endif /* TELNETSERVICE_H_ */
