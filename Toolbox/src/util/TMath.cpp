/*
 * Math.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "TMath.h"

// standard
#include <cmath>

namespace
{
static constexpr float EPS = 1E-05;
}

namespace tbox
{

bool Math::isZero(const float value)
{
	return isWithinEps(value);
}

bool Math::isWithinEps(const float value)
{
	return std::fabs(value) < EPS;
}

} // namespace tbox

