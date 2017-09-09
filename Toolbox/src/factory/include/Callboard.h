/*
 * Callboard.h
 *
 *  Created on: 4 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_CALLBOARD_H
#define TBOX_CALLBOARD_H_

// standard
#include <functional>
#include <map>
#include <memory>

// local

// project
#include "exceptionMacros.h"
#include "tboxdefs.h"

namespace tbox
{

template <typename TProductKey, typename TReturn, typename ...TArgs>
class Callboard final
{
public:

	Callboard() : m_callbacks()
	{
	}

	void registerCallback(const TProductKey key, std::function<TReturn (TArgs&&... params)> callback)
	{
		TB_ASSERT(m_callbacks.find(key) == m_callbacks.end());

		m_callbacks[key] = callback;
	}

	TReturn invoke(const TProductKey key, TArgs&&... params) const
	{
		auto iter = m_callbacks.find(key);

		TB_ASSERT(iter != m_callbacks.end());

		return iter->second(std::forward<TArgs>(params)...);
	}

private:

	std::map<TProductKey, std::function<TReturn (TArgs&&... params)>> m_callbacks;
};

} // namespace tbox

#endif /* TBOX_CALLBOARD_H */
