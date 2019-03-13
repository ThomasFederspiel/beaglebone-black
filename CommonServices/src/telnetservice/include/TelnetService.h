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
#include <vector>

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
	ICUIManager::hcui_t registerCommand(std::unique_ptr<ICUICommand> command) override;
	ICUIManager::hcui_t registerCommands(std::vector<std::unique_ptr<ICUICommand>>& commands) override;
	void unregisterCommand(const ICUIManager::hcui_t handle) override;

protected:

	void onMessage(const ServiceMessageBase& message) override;
	void onStart(ServiceAllocator& allocator) override;
	StopStatus onStop(ServiceAllocator& allocator) override;

private:
	std::unique_ptr<TelnetServer> m_telnetServer;
	std::unique_ptr<CommandExecutor> m_commandExecutor;
};

#endif /* TELNETSERVICE_H_ */
