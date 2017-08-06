/*
 *
 * linuxUtils.h
 *
 */

#ifndef TBOX_LINUX_UTILS_HPP
#define TBOX_LINUX_UTILS_HPP

// system
#include <signal.h>
#include <sys/types.h>
#include <string>
#include <vector>

extern "C" void __cxa_pure_virtual();
extern "C" void handleDivByZero(const unsigned long addr);

void linux_assert(const bool cond, char const* fmt, ...);
void linux_error(char const* fmt, ...);
bool daemonize();
std::vector<std::string> getBacktrace();
void getBacktrace(std::string& trace);
void setupFaultSignalHandlers();
void restoreFaultSignalHandlers();
void setupSignalHandler(const int signum, void (*sighandler)(int, siginfo_t *, void *));
void restoreSignalHandler(const int signum);

void setupCoredump(const char* const path);

#endif
