/*
 * BaseException.cpp
 *
 *  Created on: 16 mar 2014
 *      Author: Thomas
 */

#include "BaseException.hpp"

// project
#include "linuxUtils.h"

namespace ExceptionPkg
{
  BaseException::BaseException(const std::string& message)
    : mMessage(message)
  {
    getBacktrace(mStacktrace);
  }

  BaseException::~BaseException()
  {
  }

  const std::string& BaseException::getStacktrace() const
  {
    return mStacktrace;
  }

  const std::string& BaseException::getMessage() const
  {
    return mMessage;
  }

  void BaseException::setStacktrace()
  {
    mStacktrace = "";
  }
}
