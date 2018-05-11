/*
 * CUICommandParser.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "CUICommandParser.h"

// standard
#include <algorithm>
#include <sstream>
#include <string>

// ;+
#include <iostream>

// boost
#include "boost/regex.hpp"

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "tboxdefs.h"
#include "Utils.h"

MODULE_LOG(CUICommandParser);

namespace
{
	static const std::string CmdPathPattern = "([\\w\\d\\/\\.]+)(?:$|\\s)";
	static const std::string ValuePattern = "([+-]*[\\d]+[\\.,]?[\\d]?)(?:$|\\s)";
	static const std::string StringPattern = "\"([\\w\\d\\s\\(\\)\\-\\[\\]\\{\\}*?:;,!#+='$%&]*)\"(?:$|\\s)";
	static const std::string ValueArrayPattern = "\\[([\\d\\.,\\s+-]*)\\](?:$|\\s)";
	static const std::string OptionPattern = "-([a-zA-Z]+)(?:$|\\s)";

	static const std::string CUICommandParserPattern = StringPattern + "|" + ValuePattern
		+ "|" + OptionPattern + "|" + ValueArrayPattern + "|" + CmdPathPattern;

	static const int FullLineIndex = 0;
	static const int StringPatternIndex = 1;
	static const int ValuePatternIndex = 2;
	static const int OptionPatternIndex = 3;
	static const int ValueArrayPatternIndex = 4;
	static const int CmdPathPatternIndex = 5;

	static const std::string ArrayParserPattern = StringPattern + "|" + ValuePattern;

	static const int ArrayFullLineIndex = 0;
	static const int ArrayStringPatternIndex = 1;
	static const int ArrayValuePatternIndex = 2;
}

std::string CUICommandParser::CUICommandArg::toString() const
{
	std::ostringstream oss;

	switch (m_type)
	{
	case ArgType::Float:
		oss << m_floatValue;
		break;

	case ArgType::Int:
		oss << m_intValue;
		break;

	case ArgType::String:
		oss << m_str;
		break;

	case ArgType::CmdPath:
		oss << m_cmdPath;
		break;

	case ArgType::FloatArray:
		oss << "FloatArray";
		break;

	case ArgType::IntArray:
		oss << "IntArray";
		break;

	case ArgType::Options:
		oss << "Options";
		break;

	case ArgType::BadValue:
		oss << "BadValue";
		break;

	case ArgType::NotSetValue:
		oss << "NotSetValue";
		break;

	TB_DEFAULT(ICUICommandArg::toString(m_type));
	}

	return oss.str();
}

CUICommandParser::CUICommandParser(const std::string& line) : ICUICommandParser(),
		m_cmdPathArg(), m_optionArg(), m_arguments()
{
	parse(line);
}

bool CUICommandParser::hasCommandPath() const
{
	return m_cmdPathArg.isType(CUICommandParser::CUICommandArg::ArgType::CmdPath);
}

const std::string& CUICommandParser::getCommandPath() const
{
	TB_ASSERT(hasCommandPath());

	return m_cmdPathArg.getCmdPath();
}

bool CUICommandParser::getStringArgument(const std::size_t index, std::string& str) const
{
	if (m_arguments.size() > index)
	{
		const auto& arg = m_arguments[index];

		if (arg.isType(CUICommandParser::CUICommandArg::ArgType::String))
		{
			str = arg.getStr();
			return true;
		}
	}

	return false;
}

bool CUICommandParser::getValueArgument(const std::size_t index, int& value) const
{
	if (m_arguments.size() > index)
	{
		const auto& arg = m_arguments[index];

		if (arg.isType(CUICommandParser::CUICommandArg::ArgType::Int))
		{
			value = arg.getInt();
			return true;
		}
	}

	return false;
}

bool CUICommandParser::getValueArgument(const std::size_t index, float& value) const
{
	if (m_arguments.size() > index)
	{
		const auto& arg = m_arguments[index];

		if (arg.isType(CUICommandParser::CUICommandArg::ArgType::Float))
		{
			value = arg.getFloat();
			return true;
		}
		else if (arg.isType(CUICommandParser::CUICommandArg::ArgType::Int))
		{
			value = arg.getInt();
			return true;
		}
	}

	return false;
}

bool CUICommandParser::hasOption(const char option) const
{
	return m_optionArg.isType(CUICommandParser::CUICommandArg::ArgType::Options) && m_optionArg.getOptions().hasOption(option);
}

bool CUICommandParser::empty() const
{
	return m_arguments.empty();
}

std::size_t CUICommandParser::count() const
{
	return m_arguments.size();
}

const CUICommandParser::ICUICommandArg& CUICommandParser::operator[](const std::size_t i) const
{
	TB_ASSERT(i < m_arguments.size())

	return 	m_arguments.at(i);
}

void CUICommandParser::parse(const std::string& line)
{
    boost::regex regex(CUICommandParserPattern);

	boost::smatch what;

    auto start = line.begin();
	auto end = line.end();

	while (boost::regex_search(start, end, what, regex))
	{
		CUICommandParser::CUICommandArg arg;

		if (what[CmdPathPatternIndex].length())
		{
			auto str = what[CmdPathPatternIndex].str();

			if (!hasCommandPath())
			{
				m_cmdPathArg.setCmdPath(Utils::trim(str));
			}
			else
			{
				arg.setValue(Utils::trim(str));
			}
		}

		if (what[StringPatternIndex].length())
		{
			auto str = what[StringPatternIndex].str();
			arg.setValue(Utils::trim(str));
		}

		if (what[OptionPatternIndex].length())
		{
			parseOption(m_optionArg, what[OptionPatternIndex].str());
		}

		if (what[ValueArrayPatternIndex].length())
		{
			parseValueArray(arg, what[ValueArrayPatternIndex].str());
		}

		if (what[ValuePatternIndex].length())
		{
			parseNumber(arg, what[ValuePatternIndex].str());
		}

		if (!arg.isType(CUICommandParser::CUICommandArg::ArgType::NotSetValue))
		{
			m_arguments.push_back(arg);
		}

        start = what[FullLineIndex].second;
	}
}

void CUICommandParser::parseValueArray(CUICommandParser::CUICommandArg& arg, const std::string& str) const
{
    boost::regex regex(ArrayParserPattern);

	boost::smatch what;

    auto start = str.begin();
	auto end = str.end();

	while (boost::regex_search(start, end, what, regex))
	{
		if (what[ArrayStringPatternIndex].length())
		{
		}

		if (what[ArrayValuePatternIndex].length())
		{
		}

        start = what[ArrayFullLineIndex].second;
    }
}

void CUICommandParser::parseNumber(CUICommandParser::CUICommandArg& arg, std::string str) const
{
	str = Utils::trim(str);

	if (Utils::isFloat(str))
	{
		try
		{
			arg.setValue(stof(str));

			return;
		}
		catch (...)
		{
		}
	}

	try
	{
		arg.setValue(stoi(str));

		return;
	}
	catch (...)
	{
		arg.setBad();
	}
}

void CUICommandParser::parseOption(CUICommandParser::CUICommandArg& arg, std::string str)
{
	str = Utils::trim(str);

	std::for_each(str.begin(), str.end(), [&](char& ch) { arg.addOption(ch); });
}

