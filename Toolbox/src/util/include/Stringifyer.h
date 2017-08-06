/*
 *
 * Stringifyer.hpp
 *
 */
 
#ifndef STRINGIFYER_HPP_
#define STRINGIFYER_HPP_

// standard
#include <sstream>

#define TBOX_STRINGIFY(content) dynamic_cast<std::ostringstream&>(std::ostringstream() << content).str()

#endif /* STRINGIFYER_HPP_ */
