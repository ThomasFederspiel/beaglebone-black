/*
 * tboxutility.h
 *
 *  Created on: 21 aug 2016
 *      Author: Thomas
 */

#ifndef TBOX_TBOXUTILITY_H_
#define TBOX_TBOXUTILITY_H_

// standard
#include <chrono>
#include <functional>
#include <iterator>

namespace tbox
{
template <typename Rep, typename Period>
bool waitFor(std::function<bool(void)> pred, const std::chrono::duration<Rep, Period>& duration, std::chrono::nanoseconds pollTime = std::chrono::milliseconds(10))
{
	std::chrono::nanoseconds timePassed = std::chrono::nanoseconds(0);

	if (pollTime > duration)
	{
		pollTime = duration / 10;

		if (pollTime.count() == 0)
		{
			pollTime = duration;
		}
	}

	while (!pred() && (timePassed < duration))
	{
		std::this_thread::sleep_for(pollTime);

		timePassed += pollTime;
	}

	return pred();
}


template <typename T>
struct iterator_reverse_wrapper
{
public:

	T& iterable;
};

template <typename T>
struct const_iterator_reverse_wrapper
{
public:

	const T& iterable;
};

template <typename T>
auto begin(iterator_reverse_wrapper<T> w) -> decltype(w.iterable.rbegin())
{
	return w.iterable.rbegin();
}

template <typename T>
auto end(iterator_reverse_wrapper<T> w) -> decltype(w.iterable.rbegin())
{
	return w.iterable.rend();
}

template <typename T>
const_iterator_reverse_wrapper<T> make_reverse_iterator(const T&& iterable)
{
	return { iterable };
}

template <typename T>
iterator_reverse_wrapper<T> make_reverse_iterator(T&& iterable)
{
	return { iterable };
}

} // namespace

#endif /* TBOX_TBOXUTILITY_H_ */
