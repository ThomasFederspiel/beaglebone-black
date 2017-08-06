/*
 * BaseException.hpp
 *
 *  Created on: 16 mar 2014
 *      Author: Thomas
 */

#ifndef TBOX_BASEEXCEPTION_HPP_
#define TBOX_BASEEXCEPTION_HPP_

// standard
#include <string>

namespace ExceptionPkg
{

  class BaseException
  {
    public:
      BaseException(const std::string& message);
      virtual ~BaseException();

      const std::string& getMessage() const;
      const std::string& getStacktrace() const;

    private:
      std::string mMessage;
      std::string mStacktrace;

      void setStacktrace();
  };
}

#endif
