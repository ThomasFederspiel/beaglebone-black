/*
 * IdGenerator.h
 *
 *  Created on: 26 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_IDGENERATOR_H_
#define TBOX_IDGENERATOR_H_

template <typename T>
class IdGenerator final
{
public:

	IdGenerator()
	{
	}

	T next()
	{
		T id = s_nextId;

		++s_nextId;

		return id;
	}

private:
	static T s_nextId;
};

template<typename T >
T IdGenerator<T>::s_nextId = T();

#endif /* TBOX_IDGENERATOR_H_ */
