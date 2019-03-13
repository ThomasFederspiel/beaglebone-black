/*
 * TMath.h
 *
 *  Created on: 26 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_MATH_H_
#define TBOX_MATH_H_

namespace tbox
{
class Math final
{
public:
	static bool isZero(const float value);
	static bool isWithinEps(const float value);

	static constexpr float pi()
	{
		return PI;
	}

private:
	static constexpr float PI = 3.14159265358979f;
};
} // namespace tbox

#endif /* TBOX_MATH_H_ */
