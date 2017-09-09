/*
 * TemplateCreatorFactory.h
 *
 *  Created on: 4 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_TEMPLATECREATORFACTORY_H
#define TBOX_TEMPLATECREATORFACTORY_H_

// standard
#include <functional>
#include <map>
#include <memory>
#include <string>

// local

// project
#include "exceptionMacros.h"
#include "tboxdefs.h"

namespace tbox
{
template <typename TProduct, typename TProductKey = std::string, typename TProductCreator = std::function<std::unique_ptr<TProduct>()>>
class TemplateCreatorFactory final
{
public:

	TemplateCreatorFactory() : m_creators()
	{
	}

	void registerCreator(const TProductKey key, TProductCreator creator)
	{
		TB_ASSERT(m_creators.find(key) == m_creators.end());

		m_creators[key] = creator;
	}

	template<typename... Ts>
	std::unique_ptr<TProduct> produce(const TProductKey key, Ts&&... params) const
	{
		auto iter = m_creators.find(key);

		TB_ASSERT(iter != m_creators.end());

		return iter->second(std::forward<Ts>(params)...);
	}

private:

	std::map<TProductKey, TProductCreator> m_creators;
};

} // namespace tbox

#endif /* TBOX_TEMPLATECREATORFACTORY_H */
