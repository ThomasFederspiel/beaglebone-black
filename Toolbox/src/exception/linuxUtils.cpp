/*
 *
 * linuxUtils.cpp
 *
 */
 
#include "linuxUtils.h"

// standard
#include <iostream>

// system
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "utils.h"

MODULE_LOG(linuxUtils)

extern "C" void __cxa_pure_virtual()
{
	TB_ERROR("Pure virtual method called");
}

extern "C" void handleDivByZero(const unsigned long addr) 
{
	TB_ERROR("Division by zero");
}

void linux_assert(const bool cond, char const* fmt, ...)
{
	assert(fmt);

	if (!cond)
	{
		char buffer[200];

		va_list args;
		va_start(args, fmt);

		std::string format("Assert : ");

		format.append(fmt);

		vsnprintf(buffer, countof(buffer), format.c_str(), args);

		va_end(args);

		const std::string bufferStr(buffer);

		m_logger.log(Logger::LogLevelError, "%s", bufferStr.c_str());

		std::vector<std::string> trace = getBacktrace();

		trace.insert(trace.begin(), bufferStr);

		m_logger.errorDump(trace);

		exit(EXIT_FAILURE);
	}
}

void linux_error(char const* fmt, ...)
{
	char buffer[200];

	va_list args;
	va_start(args, fmt);

	std::string format("Assert : ");

	format.append(fmt);

	vsnprintf(buffer, countof(buffer), format.c_str(), args);

	va_end(args);

	const std::string bufferStr(buffer);

	m_logger.log(Logger::LogLevelError, "%s", bufferStr.c_str());

	std::vector<std::string> trace = getBacktrace();

	trace.insert(trace.begin(), bufferStr);

	m_logger.errorDump(trace);

	exit(EXIT_FAILURE);
}

std::vector<std::string> getBacktrace()
{
	std::vector<std::string> trace;
	
	trace.push_back(std::string("Backtrace:"));
    
	const unsigned int size = 1000u;
	 
	uint8_t* buffer = new uint8_t[size*sizeof(void*)];
	
	int nofAddresses = backtrace((void**)buffer, size);
	
	char** strings = backtrace_symbols((void**)buffer, nofAddresses);
	
	if ( strings != NULL ) 
	{
	  for ( int j = 0; j < nofAddresses; j++ )
	  {
		trace.push_back(std::string(strings[j]));
	  }
	  
	  free(strings);
	}
	else
	{
		trace.push_back(std::string("empty"));
	}     
	
	delete [] buffer;
	
  return trace;
}

void getBacktrace(std::string& trace)
{
  std::vector<std::string> strings = getBacktrace();

  trace.clear();

  for (size_t i = 0; i < strings.size(); i++)
  {
    trace.append(strings[i]);
    trace.append("\n");
  }
}

void signalHandler(int sigNum, siginfo_t* pSigInfo, void* pContext)
{
  std::vector<std::string> trace = getBacktrace();

  std::string sigNumStr;
    
  switch ( sigNum )
  {
    case SIGILL:
      sigNumStr = "SIGILL";
      break;
    case SIGSEGV:
      sigNumStr = "SIGSEGV";
      break;
    case SIGABRT:
      sigNumStr = "SIGABRT";
      break;
    case SIGBUS:
      sigNumStr = "SIGBUS";
      break;
    case SIGFPE:
      sigNumStr = "SIGFPE";
      break;
  }

  char extrainfo[20];
  snprintf(extrainfo, sizeof(extrainfo) / sizeof(extrainfo[0]), " (E:%i, C:%i)", pSigInfo->si_errno, pSigInfo->si_code);

  std::string str("Terminated by signal: ");
  str.append(sigNumStr);
  str.append(std::string(extrainfo));

  trace.insert(trace.begin(), str);

  m_logger.errorDump(trace);

  // Set up the structure to specify the default action
  struct sigaction sig_action;
  sig_action.sa_handler = SIG_DFL;
  sigemptyset (&sig_action.sa_mask);

  sigaction(SIGABRT, &sig_action, NULL);

  // Terminate with core dump
  raise(SIGABRT);
}

void setCoreDumpSize()
{
	// Set size limit for dump files
	struct rlimit limits;
	limits.rlim_cur = 196000*1024;
	limits.rlim_max = 200000*1024;
	(void)setrlimit(RLIMIT_CORE, &limits);
}

void setupSignalHandler(const int signum, void (*sighandler)(int, siginfo_t *, void *))
{
	// Set up the structure to specify the new action
	struct sigaction sig_action;
	sig_action.sa_sigaction = sighandler;
	sigemptyset (&sig_action.sa_mask);
	sig_action.sa_flags = SA_SIGINFO;

	sigaction(signum,  &sig_action, NULL);
}

void setupFaultSignalHandlers()
{
	setCoreDumpSize();

	setupSignalHandler(SIGILL, signalHandler);
	// setupSignalHandler(SIGSEGV, signalHandler);
	setupSignalHandler(SIGABRT, signalHandler);
	setupSignalHandler(SIGBUS, signalHandler);
	setupSignalHandler(SIGFPE, signalHandler);
}

void restoreSignalHandler(const int signum)
{
	// Set up the structure to specify the default action
	struct sigaction sig_action;
	sig_action.sa_handler = SIG_DFL;
	sigemptyset (&sig_action.sa_mask);

	sigaction(SIGABRT, &sig_action, NULL);
}

void restoreFaultSignalHandlers()
{
	restoreSignalHandler(SIGILL);
	// restoreSignalHandler(SIGSEGV;
	restoreSignalHandler(SIGABRT);
	restoreSignalHandler(SIGBUS);
	restoreSignalHandler(SIGFPE);
}

bool daemonize() 
{
	pid_t pid = fork();
	
	if (pid < 0) 
	{
		ERRORF("Fork failed, error = %d", errno);
		return false;
	}
	
	if (pid != 0) 
	{
		// exit parent
		_exit(EXIT_SUCCESS); 
	}

	pid_t sid = setsid();
	
	if (sid < -1) 
	{
		ERRORF("Failed to set group ID, error = %d", errno);
		return false;
	}

	// check fork for child
	pid = fork();
	
	if (pid < 0) 
	{
		ERRORF("Fork failed, error = %d", errno);
		return false;
	}
	
	if (pid != 0) 
	{
		// exit session leader
		_exit(EXIT_SUCCESS); 
	}

	umask(0002); // disable: S_IWOTH

	/* Change the current working directory */
	if (chdir("/") < 0) 
	{
		ERRORF("Failed to change work dir, error = %d", errno);
		return false;
	}

/*	for (int i = getdtablesize(); i--; )
	{
		std::cout << "Daemon 4.5 - loop" << std::endl;
		close(i);
	} */
	
	/* Close out the standard file descriptors */
	int rc = close(STDIN_FILENO);

	if (rc != 0)
	{
		ERRORF("Failed to close stdin, error = %d", errno);
		return false;
	}

	rc = close(STDOUT_FILENO);

	if (rc != 0)
	{
		ERRORF("Failed to close stdout, error = %d", errno);
		return false;
	}

	rc = close(STDERR_FILENO);

	if (rc != 0)
	{
		ERRORF("Failed to close stderr, error = %d", errno);
		return false;
	}
	
	const char *devnull = "/dev/null";
	
	stdin = fopen(devnull, "a+");
	
	if (stdin == NULL) 
	{
		ERRORF("Failed to set stdin to %s", devnull);
		return false;
	}
	
	stdout = fopen(devnull, "w");
	
	if (stdout == NULL) 
	{
		ERRORF("Failed to set stdout to %s", devnull);
		return false;
	}
	
	stderr = fopen(devnull, "w");
	
	if (stderr == NULL) 
	{
		ERRORF("Failed to set stderr to %s", devnull);
		return false;
	}
	
	return true;
}

void setupCoredump(const char* const path)
{
	// core dumps may be disallowed by parent of this process; change that
	struct rlimit core_limits;
	core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;

	INFOF("Enabling core dump");
	setrlimit(RLIMIT_CORE, &core_limits);

	std::string pattern;

	if (path != NULL)
	{
		/*  %p: pid
			%: '%' is dropped
			%%: output one '%'
			%u: uid
			%g: gid
			%s: signal number
			%t: UNIX time of dump
			%h: hostname
			%e: executable filename
		*/

		if (access(path, W_OK) == 0)
		{
			pattern.append(path);
			pattern.append("/core.%e.%p.%s.%t");
		}
		else
		{
			ERRORF("Failed to set core dump pattern as no access to %s", path);
		}
	}
	else
	{
		pattern.append("core.%e.%p.%s.%t");
	}

	if (!pattern.empty())
	{
		if (access("/proc/sys/kernel/core_pattern", W_OK) == 0)
		{
			const  int fd = open("/proc/sys/kernel/core_pattern", O_RDWR);
			write(fd, pattern.c_str(), pattern.length());
			close(fd);

			INFOF("Applying core dump pattern %s", pattern.c_str());
		}
		else
		{
			ERRORF("Failed to change core dump pattern (root priviliges?)");
		}
	}
}

