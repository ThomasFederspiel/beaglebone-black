/*
 * TemplateConstructorFactory.h
 *
 *  Created on: 4 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_TEMPLATECONSTRUCTORFACTORY_H
#define TBOX_TEMPLATECONSTRUCTORFACTORY_H_

// standard
#include <map>
#include <memory>
#include <string>

// local

// project
#include "exceptionMacros.h"
#include "tboxdefs.h"

namespace tbox
{
template <typename TProduct, typename TProductKey = std::string>
class TemplateConstructorFactory final
{
public:

	TemplateConstructorFactory() : m_creators()
	{
	}

	template <typename TConstructorClass>
	void registerClass(const TProductKey key)
	{
		static_assert(std::is_base_of<TProduct, TConstructorClass>::value, "TProduct nor base of TConstructorClass");

		TB_ASSERT(m_creators.find(key) == m_creators.end());

		m_creators[key] = tbox::make_unique<TConstructorClass>();
	}

	template <typename... Ts>
	std::unique_ptr<TProduct> produce(const TProductKey key, Ts&&... params) const
	{
		auto iter = m_creators.find(key);

		TB_ASSERT(iter != m_creators.end());

		return iter->second->construct(std::forward<Ts>(params)...);
	}

private:

	std::map<TProductKey, std::unique_ptr<TProduct>> m_creators;
};

} // namespace tbox

#endif /* TBOX_TEMPLATECONSTRUCTORFACTORY_H */
