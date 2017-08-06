/*
 * MainService.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef MAINSERVICE_H_
#define MAINSERVICE_H_


// project
#include "CapeManager.h"
#include "linuxUtils.h"
#include "MainServiceBase.h"
#include "PruController.h"
#include "PrussDriver.h"
#include "ServiceManager.h"

class MainService : public MainServiceBase
{
public:
	friend class ShutdownCommand;

	MainService();

	static void signalHandler(int sigNum, siginfo_t* pSigInfo, void* pContext);

protected:

	void onStart() override;
	void onStop() override;

private:

	void loadFragments();
	void unloadFragments();
	void setupServices();
	void startServices();
	void stopServices();
	void unregisterCUICommands();
	void registerCUICommands();
	void registerSignals();
	void unregisterSignals();
	void registerPruEvents();

	CapeManager m_capeManager;
	ServiceManager m_serviceManager;
	std::unique_ptr<PruController> m_pru0Controller;
	std::unique_ptr<PruController> m_pru1Controller;
	PrussDriver::BiDirectionPruEventChannelPtr m_biDirectionPru0EventChannel;
	PrussDriver::HostDirectionPruEventChannelPtr m_hostDirectionPru0EventChannel;
	PrussDriver::BiDirectionPruEventChannelPtr m_biDirectionPru1EventChannel;

	static MainService* s_mainService;
};

#endif /* MAINSERVICE_H_ */
