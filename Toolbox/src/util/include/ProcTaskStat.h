/*
 * ProcTaskStat.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_PROCTASKTSTAT_H_
#define TBOX_PROCTASKTSTAT_H_

// standard
#include <chrono>
#include <string>

// system
#include <unistd.h>
#include <sys/param.h>

// project

class ProcTaskStat final
{
public:

	struct taskProc
	{
		int           	  pid;                      /** The process id. **/
		char          	  exName[_POSIX_PATH_MAX];  /** The filename of the executable **/
		char          	  state;			        /** R is running, S is sleeping,
													  D is sleeping in an uninterruptible wait,
													  Z is zombie, T is traced or stopped **/
		int           	  ppid;                     /** The pid of the parent. **/
		int           	  pgrp;                     /** The pgrp of the process. **/
		int           	  session;                  /** The session id of the process. **/
		int           	  tty_nr;                   /** The tty the process uses **/
		int           	  tpgid;                    /** (too long) **/
		unsigned int	  flags;                    /** The flags of the process. **/
		unsigned long int minflt;                   /** The number of minor faults **/
		unsigned long int cminflt;                  /** The number of minor faults with childs **/
		unsigned long int majflt;                   /** The number of major faults **/
		unsigned long int cmajflt;                  /** The number of major faults with childs **/
		unsigned long int utime;                    /** user mode jiffies **/
		unsigned long int stime;                    /** kernel mode jiffies **/
		long int 		  cutime;                   /** user mode jiffies with childs **/
		long int 		  cstime;                   /** kernel mode jiffies with childs **/
		long int          priority;                 /** the standard nice value, plus fifteen **/
		long int		  nice;
		long int  		  num_threads;
		long int  		  itrealvalue;              /** The time before the next SIGALRM is sent to the process **/
		long long int     starttime; /** 22 **/     /** Time the process started after system boot **/
		unsigned long int vsize;                    /** Virtual memory size **/
		long int  		  rss;                      /** Resident Set Size **/
		unsigned long int rsslim;                   /** Current limit in bytes on the rss **/
		unsigned long int startcode;                /** The address above which program text can run **/
		unsigned long int endcode;                  /** The address below which program text can run **/
		unsigned long int startstack;               /** The address of the start of the stack **/
		unsigned long int kstkesp;                  /** The current value of ESP **/
		unsigned long int kstkeip;                  /** The current value of EIP **/
		unsigned long int signal;                   /** The bitmap of pending signals **/
		unsigned long int blocked; /** 32 **/       /** The bitmap of blocked signals **/
		unsigned long int sigignore;                /** The bitmap of ignored signals **/
		unsigned long int sigcatch;                 /** The bitmap of catched signals **/
		unsigned long int wchan;  /** 35 **/        /** (too long) **/
		unsigned long int nswap;
		unsigned long int cnswap;
		int				  exit_signal;
		int				  processor;
		unsigned int	  rt_priority;
		unsigned int	  policy;
		unsigned long long int delayacct_blkio_ticks;
		unsigned long int guest_time;
		long int 		  cguest_time;
		unsigned long int start_data;
		unsigned long int end_data;
		unsigned long int start_brk;
		unsigned long int arg_start;
		unsigned long int arg_end;
		unsigned long int env_start;
		unsigned long int env_end;
		int				  exit_code;

		std::chrono::steady_clock::time_point m_updateTime;
	};

	struct taskTime
	{
		unsigned long int utime;                    /** user mode jiffies **/
		unsigned long int stime;                    /** kernel mode jiffies **/
		long int 		  cutime;                   /** user mode jiffies with childs **/
		long int 		  cstime;                   /** kernel mode jiffies with childs **/

		float 			  cpu_usage;

		std::chrono::steady_clock::time_point m_updateTime;
	};

	explicit ProcTaskStat(const pid_t tid);

	const struct taskProc& getProcStat();
	const struct taskTime& getTaskUsage();

private:

	std::string readProcFile();

	pid_t m_tid;
	std::string m_procPath;

	struct taskProc m_taskInfo;
	struct taskTime m_taskTime;
};

#endif /* TBOX_PROCTASKTSTAT_H_ */
