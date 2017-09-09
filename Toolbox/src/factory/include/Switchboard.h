/*
 * Switchboard.h
 *
 *  Created on: 4 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_SWITCHBOARD_H
#define TBOX_SWITCHBOARD_H

// standard
#include <functional>
#include <map>
#include <memory>

// project
#include "exceptionMacros.h"
#include "tboxdefs.h"

namespace tbox
{
template <typename TProductKey, typename ...TArgs>
class Switchboard final
{
public:

	Switchboard() : m_actions()
	{
	}

	template <typename TConstructorClass, typename TAction>
	void registerAction(const TProductKey key, TAction action)
	{
		TB_ASSERT(m_actions.find(key) == m_actions.end());

		m_actions[key] = tbox::make_unique<Wrapper<TConstructorClass, TAction>>(action);
	}

	void invoke(const TProductKey key, TArgs... params)
	{
		auto iter = m_actions.find(key);

		TB_ASSERT(iter != m_actions.end());

		iter->second->invoke(params...);
	}

private:

	class WrapperBase
	{
	public:
		virtual ~WrapperBase() = default;
		virtual void invoke(TArgs... params) = 0;
	};

	template <typename T, typename A>
	class Wrapper final : public WrapperBase
	{
	public:

		explicit Wrapper(A action) : m_action(action)
		{
		}

		void invoke(TArgs... params)
		{
			T t(params...);
			m_action(t);
		}

	private:
		A m_action;
	};

	std::map<TProductKey, std::unique_ptr<WrapperBase>> m_actions;
};

} // namespace tbox

#endif /* TBOX_SWITCHBOARD_H */
