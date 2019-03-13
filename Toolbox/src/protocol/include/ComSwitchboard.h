/*
 * ComSwitchboard.h
 *
 *  Created on: 4 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_COMSWITCHBOARD_H
#define TBOX_COMSWITCHBOARD_H

// standard
#include <functional>
#include <map>
#include <memory>

// project
#include "exceptionMacros.h"
#include "stdExtension.h"

namespace tbox
{
template <typename TProductKey, typename TConnection, typename ...TArgs>
class ComSwitchboard final
{
public:

	ComSwitchboard() : m_actions()
	{
	}

	template <typename TConstructorClass, typename TAction>
	void registerAction(const TProductKey key, TAction action)
	{
		TB_ASSERT(m_actions.find(key) == m_actions.end());

		m_actions[key] = std::make_unique<Wrapper<TConstructorClass, TAction>>(action);
	}

	void invoke(const TProductKey key, TConnection connection, TArgs... params)
	{
		auto iter = m_actions.find(key);

		TB_ASSERT(iter != m_actions.end());

		iter->second->invoke(connection, params...);
	}

private:

	class WrapperBase
	{
	public:
		virtual ~WrapperBase() = default;
		virtual void invoke(TConnection connection, TArgs... params) = 0;
	};

	template <typename T, typename A>
	class Wrapper final : public WrapperBase
	{
	public:

		explicit Wrapper(A action) : m_action(action)
		{
		}

		void invoke(TConnection connection, TArgs... params)
		{
			T t(params...);
			m_action(connection, t);
		}

	private:
		A m_action;
	};

	std::map<TProductKey, std::unique_ptr<WrapperBase>> m_actions;
};

} // namespace tbox

#endif /* TBOX_COMSWITCHBOARD_H */
