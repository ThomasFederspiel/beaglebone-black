/*
 * DirectoryTree.h
 *
 *  Created on: 4 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_HISTORY_H_
#define TBOX_HISTORY_H_

// standard
#include <list>
#include <optional>

namespace tbox
{

template <typename T, typename std::size_t SIZE>
class History
{
public:
	History() : m_history(), m_markPos(m_history.begin())
	{
	}

	void add(const T& value);
	const std::optional<T>& current() const;
	const std::optional<T>& prev() const;
	const std::optional<T>& next() const;

private:
	bool erase(const T& value);

	std::list<T> m_history;
	typename std::list<T>::iterator m_insertPos;
	typename std::list<T>::iterator m_markPos;
};


template <typename T, typename std::size_t SIZE>
void History<T, SIZE>::add(const T& value)
{
	erase(value);

	m_history.insert(m_insertPos, value);

	if (m_history.size() >= SIZE)
	{
		m_history.pop_front();
	}

	m_markPos = m_history.begin();
}

template <typename T, typename std::size_t SIZE>
bool History<T, SIZE>::erase(const T& value)
{
	for (auto iter = m_history.begin(); iter != m_history.end(); ++iter)
	{
		if (*iter == value)
		{
			m_history.erase(iter);

			return true;
		}
	}

	return false;
}

template <typename T, typename std::size_t SIZE>
const std::optional<T>& History<T, SIZE>::current() const
{
	if (m_history.empty() || (m_markPos == m_history.end()))
	{
		return std::optional<T>();
	}

	return std::optional<T>(*m_markPos);
}

template <typename T, typename std::size_t SIZE>
const std::optional<T>& History<T, SIZE>::prev() const
{
	if (m_history.empty())
	{
		return std::optional<T>();
	}

	if (m_markPos == m_history.begin())
	{
		return std::optional<T>();
	}
	else
	{
		--m_markPos;
	}

	return std::optional<T>(*m_markPos);
}

template <typename T, typename std::size_t SIZE>
const std::optional<T>& History<T, SIZE>::next() const
{
	if (m_history.empty())
	{
		return std::optional<T>();
	}

	if (m_markPos != m_history.end())
	{
		++m_markPos;
	}

	if (m_markPos != m_history.end())
	{
		return std::optional<T>(*m_markPos);
	}

	return std::optional<T>();
}

} // namespace tbox

#endif /* TBOX_HISTORY_H_ */
