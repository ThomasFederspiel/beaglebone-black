/*
 *
 * exceptionMacros.h
 *
 */

#ifndef TBOX_EXCEPTIONMACROS_H
#define TBOX_EXCEPTIONMACROS_H

// standard
#include <sstream>

// boost
#include <boost/config.hpp>

// project
#include "linuxUtils.h"

#define TB_ERROR(v) linux_error("%s(%d) - %s", __FILE__, __LINE__, dynamic_cast<std::ostringstream&>(std::ostringstream() << v).str().c_str())
#define TB_ERRORF(fmt, args...) linux_error("%s(%d) - " fmt, __FILE__, __LINE__, ##args)
#define TB_DEFAULT(v) default : TB_ERROR(dynamic_cast<std::ostringstream&>(std::ostringstream("Switch default reached. Unknown value ") << v).str())

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

// ;+ #define TBOX_ENABLE_ASSERT_HANDLER

#ifdef TBOX_ENABLE_ASSERT_HANDLER

extern void tbAssertHook();

#define TB_ASSERT_1(expr)					\
		do									\
		{									\
			if (BOOST_UNLIKELY(!(expr)))	\
			{								\
				tbAssertHook();				\
				TB_ERROR(#expr);			\
			}								\
		} while (0);

#define TB_ASSERT_2(expr, msg)				\
		do									\
		{									\
			if (BOOST_UNLIKELY(!(expr)))	\
			{								\
				tbAssertHook();				\
				TB_ERROR(msg);				\
			}								\
		} while (0);

#define TB_ASSERT_3(expr, fmt, msg)			\
		do									\
		{									\
			if (BOOST_UNLIKELY(!(expr)))	\
			{								\
				tbAssertHook();				\
				TB_ERROR(fmt, msg);			\
			}								\
		} while (0);
#else
#define TB_ASSERT_1(expr)					\
		do									\
		{									\
			if (BOOST_UNLIKELY(!(expr)))	\
			{								\
				TB_ERROR(#expr);			\
			}								\
		} while (0);

#define TB_ASSERT_2(expr, msg)				\
		do									\
		{									\
			if (BOOST_UNLIKELY(!(expr)))	\
			{								\
				TB_ERROR(msg);				\
			}								\
		} while (0);

#define TB_ASSERT_3(expr, fmt, msg)			\
		do									\
		{									\
			if (BOOST_UNLIKELY(!(expr)))	\
			{								\
				TB_ERROR(fmt, msg);			\
			}								\
		} while (0);
#endif

// The interim macro that strips the excess and ends up with the required macro
// TB_ASSERT_X macro
#define TB_ASSERT_X(_1, _2, _3, _, ...) _

#define TB_ASSERT(...)										\
		TB_ASSERT_X(__VA_ARGS__, TB_ASSERT_3(__VA_ARGS__),	\
				TB_ASSERT_2(__VA_ARGS__),					\
				TB_ASSERT_1(__VA_ARGS__), 0)

#endif
