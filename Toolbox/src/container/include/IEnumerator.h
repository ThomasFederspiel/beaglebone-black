/*
 * IEnumerator.h
 *
 *  Created on: 4 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_IENUMERATOR_H_
#define TBOX_IENUMERATOR_H_

namespace tbox
{

template <typename T>
class IEnumerator
{
public:

	virtual ~IEnumerator()
	{
	}

	virtual bool hasMore() = 0;
	virtual T next() = 0;
};

} // namespace tbox

#endif /* TBOX_IENUMERATOR_H_ */
