/*
 * StdExtension.h
 *
 *  Created on: 26 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_STDEXTENSION_H_
#define TBOX_STDEXTENSION_H_

#include <type_traits>
#include <memory>
#include <chrono>

namespace std
{
#if __cplusplus < 201402L

	template<typename T, typename... Ts>
	inline std::unique_ptr<T> make_unique(Ts&&... params)
	{
		return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
	}

#endif

#if __cplusplus <= 201402L

	template< bool B, class T = void >
	  using enable_if_t = typename enable_if<B,T>::type;

	namespace chrono
	{
		template <class T> struct is_duration : std::false_type {};
		template <class Rep, class Period> struct is_duration<
			chrono::duration<Rep, Period>> : std::true_type {};

		template <class To, class Rep, class Period, class = enable_if_t<is_duration<To>{}>>
		To floor(const duration<Rep, Period>& d)
		{
			To t = duration_cast<To>(d);
			if (t > d)
			{
				return t - To{1};
			}

			return t;
		}

		template <class To, class Rep, class Period,
				  class = enable_if_t<is_duration<To>{} &&
						 !treat_as_floating_point<typename To::rep>{}>>
		To round(const duration<Rep, Period>& d)
		{
			To t0 = floor<To>(d);
			To t1 = t0 + To{1};
			auto diff0 = d - t0;
			auto diff1 = t1 - d;

			if (diff0 == diff1)
			{
				if (t0.count() & 1)
				{
					return t1;
				}

				return t0;
			}
			else if (diff0 < diff1)
			{
				return t0;
			}

			return t1;
		}
	}

#endif
}

#endif /* TBOX_STDEXTENSION_H_ */
