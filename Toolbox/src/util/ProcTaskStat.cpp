/*
 * ProcTaskStat.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "ProcTaskStat.h"

// standard
#include <chrono>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdio.h>

// system
#include <unistd.h>
#include <sys/types.h>

// project
#include "exceptionMacros.h"

namespace
{
const static std::string ProcTaskStatTmpl = "/proc/%d/task/%d/stat";

std::string getProcPath(const pid_t tid)
{
	char* strp = nullptr;

	const auto pid = getpid();

	const auto ret = asprintf(&strp, ProcTaskStatTmpl.c_str(), pid, tid);

	TB_ASSERT(ret >= 0, "asprintf failed with error %d", ret);

	std::string str(strp);

	free(strp);

	return str;
}
}

ProcTaskStat::ProcTaskStat(const pid_t tid) : m_tid(tid), m_procPath(getProcPath(tid)),
	m_taskInfo(), m_taskTime()
{
	getTaskUsage();
}

const struct ProcTaskStat::taskProc& ProcTaskStat::getProcStat()
{
	std::stringstream statStream(readProcFile());

	statStream >> m_taskInfo.pid;
	statStream >> m_taskInfo.exName;
	statStream >> m_taskInfo.state;
	statStream >> m_taskInfo.ppid;
	statStream >> m_taskInfo.pgrp;
	statStream >> m_taskInfo.session;
	statStream >> m_taskInfo.tty_nr;
	statStream >> m_taskInfo.tpgid;
	statStream >> m_taskInfo.flags;
	statStream >> m_taskInfo.minflt;
	statStream >> m_taskInfo.cminflt;
	statStream >> m_taskInfo.majflt;
	statStream >> m_taskInfo.cmajflt;
	statStream >> m_taskInfo.utime;
	statStream >> m_taskInfo.stime;
	statStream >> m_taskInfo.cutime;
	statStream >> m_taskInfo.cstime;
	statStream >> m_taskInfo.priority;
	statStream >> m_taskInfo.nice;
	statStream >> m_taskInfo.num_threads;
	statStream >> m_taskInfo.itrealvalue;
	statStream >> m_taskInfo.starttime;
	statStream >> m_taskInfo.vsize;
	statStream >> m_taskInfo.rss;
	statStream >> m_taskInfo.rsslim;
	statStream >> m_taskInfo.startcode;
	statStream >> m_taskInfo.endcode;
	statStream >> m_taskInfo.startstack;
	statStream >> m_taskInfo.kstkesp;
	statStream >> m_taskInfo.kstkeip;
	statStream >> m_taskInfo.signal;
	statStream >> m_taskInfo.blocked;
	statStream >> m_taskInfo.sigignore;
	statStream >> m_taskInfo.sigcatch;
	statStream >> m_taskInfo.wchan;
	statStream >> m_taskInfo.nswap;
	statStream >> m_taskInfo.cnswap;
	statStream >> m_taskInfo.exit_signal;
	statStream >> m_taskInfo.processor;
	statStream >> m_taskInfo.rt_priority;
	statStream >> m_taskInfo.policy;
	statStream >> m_taskInfo.delayacct_blkio_ticks;
	statStream >> m_taskInfo.guest_time;
	statStream >> m_taskInfo.cguest_time;
	statStream >> m_taskInfo.start_data;
	statStream >> m_taskInfo.end_data;
	statStream >> m_taskInfo.start_brk;
	statStream >> m_taskInfo.arg_start;
	statStream >> m_taskInfo.arg_end;
	statStream >> m_taskInfo.env_start;
	statStream >> m_taskInfo.env_end;
	statStream >> m_taskInfo.exit_code;

	m_taskInfo.m_updateTime = std::chrono::steady_clock::now();

	return m_taskInfo;
}

#include <iostream>

const struct ProcTaskStat::taskTime& ProcTaskStat::getTaskUsage()
{
	std::stringstream statStream(readProcFile());

	const auto newUpdateTime = std::chrono::steady_clock::now();

	std::size_t field = 13;

	while (statStream.good() && field)
	{
		statStream.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
		--field;
	}

	if (statStream.good())
	{
		const auto prevTaskTime = m_taskTime.utime + m_taskTime.stime + m_taskTime.cutime + m_taskTime.cstime;

		statStream >> m_taskTime.utime;
		statStream >> m_taskTime.stime;
		statStream >> m_taskTime.cutime;
		statStream >> m_taskTime.cstime;

		const auto newTaskTime = m_taskTime.utime + m_taskTime.stime + m_taskTime.cutime + m_taskTime.cstime;

		const float clockTick_ms = 1000.f / sysconf(_SC_CLK_TCK);

		m_taskTime.cpu_usage = ((newTaskTime - prevTaskTime) * clockTick_ms) * 100.f /
				std::chrono::duration_cast<std::chrono::milliseconds>(newUpdateTime - m_taskTime.m_updateTime).count();

		m_taskTime.m_updateTime = newUpdateTime;
	}

	return m_taskTime;
}

std::string ProcTaskStat::readProcFile()
{
	std::string statStr;

	std::ifstream file;

	file.open(m_procPath.c_str());

	if (file)
	{
		std::getline(file, statStr);

		file.close();
	}

	return statStr;
}
