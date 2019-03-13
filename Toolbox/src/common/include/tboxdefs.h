/*
 * tboxdef.h
 *
 *  Created on: 21 aug 2016
 *      Author: Thomas
 */

#ifndef TBOX_TBOXDEFS_H_
#define TBOX_TBOXDEFS_H_

// standard
#include <cstddef>
#include <memory>

namespace tbox
{
	template<typename T>
	inline std::unique_ptr<T[]> make_unique_array(const std::size_t size)
	{
		return std::unique_ptr<T[]>(new T[size]);
	}

	template<typename Base, typename T>
	inline bool instanceof(const T* ptr)
	{
	    return dynamic_cast<const Base*>(ptr) != nullptr;
	}

	template<typename Base, typename T>
	inline bool instanceof(T* ptr, Base*& out)
	{
	    out = dynamic_cast<Base*>(ptr);

	    return out != nullptr;
	}

	template<typename Base, typename T>
	inline bool instanceof(const T& ref)
	{
	    return dynamic_cast<const Base*>(&ref) != nullptr;
	}

	template<typename Base, typename T>
	inline bool instanceof(T& ref, Base*& out)
	{
	    out = dynamic_cast<Base*>(&ref);

	    return out != nullptr;
	}
}

#endif /* TBOX_TBOXDEFS_H_ */
