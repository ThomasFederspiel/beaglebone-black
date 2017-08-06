/*
 * NetManageMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef NETMANAGEMESSAGE_H_
#define NETMANAGEMESSAGE_H_

// project
#include "CommonMessageTypes.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

class NetManageMessage final : public ServiceMessageBase
{
public:

	enum class Action
	{
		UndefinedAction = 0,
		IfUpAction,
		IfDownAction,
	};

	static NetManageMessage createIfUpAction(const std::string& interface)
	{
		NetManageMessage message;

		message.m_action = Action::IfUpAction;
		message.m_interface = interface;

		return message;
	}

	Action getAction() const
	{
		return m_action;
	}

	const std::string& getInterfacce() const
	{
		return m_interface;
	}

	const std::string& getIpAddress() const
	{
		return m_ipAddress;
	}

	const std::string& getMask() const
	{
		return m_mask;
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return tbox::make_unique<NetManageMessage>(*this);
	}

	static const char* toString(const Action action)
	{
		#define CASE(v) case v: return #v;
		switch (action)
		{
			CASE(Action::UndefinedAction)
			CASE(Action::IfUpAction)
			CASE(Action::IfDownAction)
		}
		static std::string tmp = std::to_string(static_cast<int>(action));
		return tmp.c_str();
		#undef CASE
	}

private:
	explicit NetManageMessage() : ServiceMessageBase(commonservices::CommonMessageTypes::Type::NetManageMessage),
		m_action(Action::UndefinedAction), m_interface(), m_ipAddress(), m_mask()
	{
	}

	Action m_action;
	std::string m_interface;
	std::string m_ipAddress;
	std::string m_mask;
};

#endif /* MOTIONMESSAGE_H_ */
