
// local
#include "MainService.h"

// project
#include "linuxUtils.h"
#include "logger.h"
#include "SchedulerUtil.h"
#include "ServiceMessageBase.h"

MODULE_LOG("main");

/* main */
int main(int ac, char** av)
{
	CONFIGURE_LOG(motorappl, Logger::LogLevelInfo, Logger::FileDest | Logger::StdoutDest);

	TB_ASSERT(SchedulerUtil::supportRRScheduler(true));

	MainService mainService;

	setupFaultSignalHandlers();

	mainService.run();

	restoreFaultSignalHandlers();

	return 0;
}
