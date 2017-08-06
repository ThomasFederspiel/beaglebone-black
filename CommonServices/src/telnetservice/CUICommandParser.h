/*
 * CUICommandParser.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef CUICOMMANDPARSER_H_
#define CUICOMMANDPARSER_H_

// standard
#include <type_traits>
#include <vector>

// project
#include "exceptionMacros.h"
#include "ICUICommandParser.h"

class CUICommandParser final : public ICUICommandParser
{
public:

	class CUICommandArg final  : public ICUICommandParser::ICUICommandArg
	{
	public:
		friend class CUICommandParser;

		CUICommandArg() : m_str(), m_cmdPath(), m_floatValue(0.f), m_intValue(0),
			m_type(ArgType::NotSetValue)
		{
		}

		const std::string& getCmdPath() const override
		{
			TB_ASSERT(m_type == ArgType::CmdPath);
			return m_cmdPath;
		}

		const std::string& getStr() const override
		{
			TB_ASSERT(m_type == ArgType::String);
			return m_str;
		}

		float getFloat() const override
		{
			TB_ASSERT(m_type == ArgType::Float);
			return m_floatValue;
		}

		int getInt() const override
		{
			TB_ASSERT(m_type == ArgType::Int);
			return m_intValue;
		}

		const OptionArg& getOptions() const override
		{
			TB_ASSERT(m_type == ArgType::Options);
			return m_options;
		}

		bool isType(const ArgType type) const override
		{
			return m_type == type;
		}

		std::string toString() const override;

	private:

		void setCmdPath(const std::string& path)
		{
			m_cmdPath = path;
			m_type = ArgType::CmdPath;
		}

		void setValue(const std::string& str)
		{
			m_str = str;
			m_type = ArgType::String;
		}

		void setValue(const int value)
		{
			m_intValue = value;
			m_type = ArgType::Int;
		}

		void setValue(const float value)
		{
			m_floatValue = value;
			m_type = ArgType::Float;
		}

		void setBad()
		{
			m_type = ArgType::BadValue;
		}

		void addOption(const char option)
		{
			m_options.add(option);
			m_type = ArgType::Options;
		}

		std::string m_str;
		std::string m_cmdPath;
		float m_floatValue;
		int m_intValue;
		OptionArg m_options;
		ArgType m_type;
	};

	explicit CUICommandParser(const std::string& line);

	bool hasCommandPath() const override;
	const std::string& getCommandPath() const override;
	bool hasOption(const char option) const override;
	bool getStringArgument(const std::size_t index, std::string& str) const override;
	bool getValueArgument(const std::size_t index, int& value) const override;
	bool getValueArgument(const std::size_t index, float& value) const override;

	bool empty() const override;
	std::size_t count() const override;
	const ICUICommandArg& operator[](const std::size_t i) const override;

private:
	void parse(const std::string& line);
	void parseNumber(CUICommandParser::CUICommandArg& arg, std::string str) const;
	void parseValueArray(CUICommandParser::CUICommandArg& arg, const std::string& str) const;
	void parseOption(CUICommandParser::CUICommandArg& arg, std::string str);

	CUICommandArg m_cmdPathArg;
	CUICommandArg m_optionArg;
	std::vector<CUICommandArg> m_arguments;
};

#endif /* CUICOMMANDPARSER_H_ */
