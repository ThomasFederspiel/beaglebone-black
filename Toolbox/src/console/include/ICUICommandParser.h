/*
 * ICUICommandParser.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_ICUICOMMANDPARSER_H_
#define TBOX_ICUICOMMANDPARSER_H_

#include <vector>

class ICUICommandParser
{
public:

	class ICUICommandArg
	{
	public:
		enum class ArgType
		{
			Float = 0,
			Int,
			String,
			CmdPath,
			FloatArray,
			IntArray,
			Options,
			NotSetValue,
			BadValue
		};

		class OptionArg final
		{
		public:

			void add(const char option)
			{
				if (!hasOption(option))
				{
					m_options.push_back(option);
				}
			}

			bool hasOption(const char option) const
			{
				for (const auto& item : m_options)
				{
					if (item == option)
					{
						return true;
					}
				}

				return false;
			}

		private:
			std::vector<char> m_options;
		};

		virtual ~ICUICommandArg()
		{
		}

		virtual const std::string& getCmdPath() const = 0;
		virtual const std::string& getStr() const = 0;
		virtual float getFloat() const = 0;
		virtual int getInt() const = 0;
		virtual const OptionArg& getOptions() const = 0;
		virtual bool isType(const ArgType type) const = 0;
		virtual std::string toString() const = 0;

		static const char* toString(const ArgType type)
		{
			#define CASE(v) case v: return #v;
			switch (type)
			{
				CASE(ArgType::Float)
				CASE(ArgType::Int)
				CASE(ArgType::String)
				CASE(ArgType::CmdPath)
				CASE(ArgType::FloatArray)
				CASE(ArgType::IntArray)
				CASE(ArgType::Options)
				CASE(ArgType::NotSetValue)
				CASE(ArgType::BadValue)
			}
			static std::string tmp = std::to_string(static_cast<int>(type));
			return tmp.c_str();
			#undef CASE
		}

	};

	virtual ~ICUICommandParser()
	{
	}

	virtual bool hasCommandPath() const = 0;
	virtual const std::string& getCommandPath() const = 0;
	virtual bool hasOption(const char option) const = 0;
	virtual bool getStringArgument(const std::size_t index, std::string& str) const = 0;
	virtual bool getValueArgument(const std::size_t index, int& value) const = 0;
	virtual bool getValueArgument(const std::size_t index, float& value) const = 0;
	virtual bool empty() const = 0;
	virtual std::size_t count() const = 0;
	virtual const ICUICommandArg& operator[](const std::size_t i) const = 0;
};

#endif /* TBOX_ICUICOMMANDPARSER_H_ */
