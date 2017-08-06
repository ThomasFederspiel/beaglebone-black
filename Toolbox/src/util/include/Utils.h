/*
 * Utils.h
 *
 *  Created on: 26 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_UTILS_H_
#define TBOX_UTILS_H_

// standard
#include <cstdlib>
#include <sstream>
#include <string>

template <typename T, std::size_t N>
constexpr std::size_t countof(T const (&)[N]) noexcept
{
	return N;
}

class Utils final
{
public:

	static bool isDigits(std::string str);

	static bool split(std::string& str, std::string& token, const std::string& delimiter);
	static std::string trim(std::string& str);

	template <typename T>
	static T fromHexStr(const std::string& str);

	static bool mkdir(const std::string& path, const bool recursive);

	static std::string ensureEnd(std::string str, const std::string& ensureStr);
	static std::string trim(std::string str, const char trimChar);
	static std::string trimEnd(std::string str, const char trimChar);
	static std::string trimFront(std::string str, const char trimChar);
	static std::string concatPath(std::string str1, std::string str2, const char separator = '/');
	static bool systemExec(const std::string& cmd, std::string& output);
};

template <typename T>
T Utils::fromHexStr(const std::string& str)
{
	T x;

    std::stringstream ss;
    ss << std::hex << str;
    ss >> x;

    return x;
}

#endif /* TBOX_UTILS_H_ */
