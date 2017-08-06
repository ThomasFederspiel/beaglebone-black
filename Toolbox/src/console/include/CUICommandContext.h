/*
 * CUICommandContext.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_CUICOMMANDCONTEXT_H_
#define TBOX_CUICOMMANDCONTEXT_H_

// standard
#include <memory>

// project
#include "exceptionMacros.h"

// local
#include "ICUICommandContext.h"

class CUICommandContext
{
public:

	explicit CUICommandContext(std::shared_ptr<ICUICommandContext> impl) : m_impl(impl)
	{
		TB_ASSERT(m_impl);
	}

	const std::string& newline() const
	{
		return m_impl->newline();
	}

	std::ostringstream& output()
	{
		return m_impl->output();
	}

	void finalizeError(const std::string&& message)
	{
		m_impl->finalizeError(std::move(message));
	}

	void finalize()
	{
		m_impl->finalize();
	}

	ICUICommandContext& getImpl()
	{
		return *m_impl;
	}

private:
	std::shared_ptr<ICUICommandContext> m_impl;
};

#endif /* TBOX_CUICOMMANDCONTEXT_H_ */
