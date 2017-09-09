/*
 * TemplateConstructorFactory_v2.h
 *
 *  Created on: 4 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_TEMPLATECONSTRUCTORFACTORY_V2_H
#define TBOX_TEMPLATECONSTRUCTORFACTORY_V2_H_

// standard
#include <map>
#include <memory>

// local

// project
#include "exceptionMacros.h"
#include "tboxdefs.h"

namespace tbox
{
template <typename TProduct, typename TProductKey, typename ...TArgs>
class TemplateConstructorFactory_v2 final
{
public:

	TemplateConstructorFactory_v2() : m_creators()
	{
	}

	template <typename TConstructorClass>
	void registerClass(const TProductKey key)
	{
		static_assert(std::is_base_of<TProduct, TConstructorClass>::value, "TProduct nor base of TConstructorClass");

		TB_ASSERT(m_creators.find(key) == m_creators.end());

		m_creators[key] = tbox::make_unique<Wrapper<TConstructorClass>>();
	}

	std::unique_ptr<TProduct> produce(const TProductKey key, TArgs... params) const
	{
		auto iter = m_creators.find(key);

		TB_ASSERT(iter != m_creators.end());

		return iter->second->create(params...);
	}

private:

	class WrapperBase
	{
	public:
		virtual ~WrapperBase() = default;
		virtual std::unique_ptr<TProduct> create(TArgs... params) = 0;
	};

	template <typename T>
	class Wrapper final : public WrapperBase
	{
	public:
		std::unique_ptr<TProduct> create(TArgs... params)
		{
			return tbox::make_unique<T>(params...);
		}
	};

	std::map<TProductKey, std::unique_ptr<WrapperBase>> m_creators;
};

} // namespace tbox

#endif /* TBOX_TEMPLATECONSTRUCTORFACTORY_V2_H */
