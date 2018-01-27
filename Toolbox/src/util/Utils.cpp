/*
 * Utils.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "Utils.h"

// standard
#include <array>
#include <cstdio>
#include <memory>
#include <string>

// system
#include <sys/stat.h>
#include <unistd.h>

// boost
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

bool Utils::isDigits(std::string str)
{
	return trim(str).find_first_not_of("0123456789") == std::string::npos;
}

bool Utils::isFloat(std::string str)
{
	int index = 0;

	trim(str);

	if ((str.at(index) == '-') || (str.at(index) == '+'))
	{
		++index;
	}

	std::string valStr = str.substr(index);

	auto dotIndex = valStr.find_first_of(".");

	if (dotIndex == std::string::npos)
	{
		return false;
	}

	if (valStr.substr(0, dotIndex).find_first_not_of("0123456789") != std::string::npos)
	{
		return false;
	}

	if ((valStr.length() > (dotIndex + 1)) && (valStr.substr(dotIndex + 1, dotIndex).find_first_not_of("0123456789") != std::string::npos))
	{
		return false;
	}

	return true;
}

bool Utils::split(std::string& str, std::string& token, const std::string& delimiter)
{
	const auto pos = str.find(delimiter);

	if (pos != std::string::npos)
	{
		token = str.substr(0, pos);
		str.erase(0, pos + delimiter.length());
	}

	return pos != std::string::npos;
}

std::string Utils::trim(std::string& str)
{
	boost::trim(str);

	return str;
}

std::string Utils::trim(std::string str, const char trimChar)
{
	return trimFront(trimEnd(str, trimChar), trimChar);
}

std::string Utils::trimEnd(std::string str, const char trimChar)
{
	std::size_t i = str.size();

	if (i > 0)
	{
		do
		{
			--i;

			if (str[i] != trimChar)
			{
				str.erase(str.begin() + i + 1, str.end());

				return str;
			}
		}
		while (i > 0);
	}

	str.clear();

	return str;
}

std::string Utils::trimFront(std::string str, const char trimChar)
{
	std::size_t i = 0;

	while (i < str.size())
	{
		if (str[i] != trimChar)
		{
			str.erase(str.begin(), str.begin() + i);

			return str;
		}

		++i;
	}

	str.clear();

	return str;
}

std::string Utils::ensureEnd(std::string str, const std::string& ensureStr)
{
	const std::size_t i = str.size();

	if (i > 0)
	{
		const auto pos = str.find(ensureStr);

		if (pos != std::string::npos)
		{
			if (pos + ensureStr.size() == i)
			{
				return str;
			}
		}
	}

	return str + ensureStr;
}

std::string Utils::concatPath(std::string str1, std::string str2, const char separator)
{
	return trimEnd(trim(str1), separator) + separator + trimFront(trim(str2), separator);
}

bool Utils::mkdir(const std::string& path, const bool recursive)
{
	if (!path.empty())
	{
		if (access(path.c_str(), W_OK) == 0)
		{
			return true;
		}

		if (recursive)
		{
			return boost::filesystem::create_directories(path);
		}
		else
		{
			return boost::filesystem::create_directory(path);
		}
	}

	return false;
}

bool Utils::systemExec(const std::string& cmd, std::string& output)
{
	bool success = false;

	output.clear();

    std::array<char, 128> buffer;

    std::unique_ptr<FILE> pipe(popen(cmd.c_str(), "r"));

    if (!pipe)
	{
    	return success;
	}

    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), buffer.max_size(), pipe.get()) != nullptr)
        {
            output += buffer.data();
        }
    }

    int code = pclose(pipe.get());

    if (code == -1)
    {
    	success = false;
    }

    return success;
}

