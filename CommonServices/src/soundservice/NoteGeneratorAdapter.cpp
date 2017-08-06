/*
 * NoteGeneratorAdapter.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "NoteGeneratorAdapter.h"

// standard
#include <thread>

// project
#include "Logger.h"

MODULE_LOG(NoteGeneratorAdapter);

namespace
{
static NoteGeneratorAdapter::DefaultContext s_defaultContext;
}

NoteGeneratorAdapter::NoteGeneratorAdapter(IIPCDeviceEPwmProxy& epwmProxy,
		IIPCDevicePwmsProxy& pwmsProxy)
	: NoteGeneratorAdapter(epwmProxy, pwmsProxy, s_defaultContext)
{
}

NoteGeneratorAdapter::NoteGeneratorAdapter(IIPCDeviceEPwmProxy& epwmProxy,
		IIPCDevicePwmsProxy& pwmsProxy, INoteGeneratorContext& context)
	: m_ipcDeviceEPwmProxy(epwmProxy), m_ipcDevicePwmsProxy(pwmsProxy), m_context(context)
{
}

void NoteGeneratorAdapter::DefaultContext::wait(const std::chrono::milliseconds& duration)
{
	std::this_thread::sleep_for(duration);
}

void NoteGeneratorAdapter::open()
{
	m_ipcDevicePwmsProxy.open();
	m_ipcDeviceEPwmProxy.open();

	const uint16_t pwmPeriod = m_ipcDeviceEPwmProxy.armFreq(PlayNote::MinFreq);
	m_ipcDeviceEPwmProxy.getDefaultChannel().setDuty(pwmPeriod >> 1);
}

void NoteGeneratorAdapter::close()
{
	m_ipcDeviceEPwmProxy.close();
	m_ipcDevicePwmsProxy.close();
}

void NoteGeneratorAdapter::wait(const uint16_t duration)
{
	m_context.wait(std::chrono::milliseconds(duration));
}

void NoteGeneratorAdapter::playNote(const uint16_t freq, const uint16_t duration, const uint16_t pause)
{
	if (freq == 0)
	{
		const bool high = false;
		m_ipcDeviceEPwmProxy.getDefaultChannel().disable(high);

		wait(duration);
	}
	else
	{
		const uint16_t pwmPeriod = m_ipcDeviceEPwmProxy.setFreq(freq);
		m_ipcDeviceEPwmProxy.getDefaultChannel().setDuty(pwmPeriod >> 1);

		m_ipcDeviceEPwmProxy.getDefaultChannel().enable();

		wait(duration);

		const bool high = false;
		m_ipcDeviceEPwmProxy.getDefaultChannel().disable(high);
	}

	wait(pause);
}
