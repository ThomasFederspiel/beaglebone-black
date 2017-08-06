/*
 * CapeManager.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "CapeManager.h"

#include <fstream>
#include <sstream>
#include <regex>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "exceptionMacros.h"
#include "Logger.h"
#include "Utils.h"

MODULE_LOG(CapeManager)

namespace
{
static const char* const DefaultFragmentVersion = "00A0";
static const char* const DefaultFragmentExtension = ".dtbo";
static const char* const BoneSysFile = "/sys/devices/bone_capemgr.9/slots";
static const char* const FragmentLibDir = "/lib/firmware";
}

bool CapeManager::loadFragment(const std::string& name)
{
	bool loaded = false;

	if (isFragmentLoaded(name))
	{
		loaded = true;
	}
	else
	{
		if (existsFragmentFile(name))
		{
			std::ofstream out;

			out.open(BoneSysFile, std::ios::out | std::ios::app);

			if (out)
			{
				out << name << std::endl;
				out.close();

				loaded = true;
			}
			else
			{
				ERROR("Can't open sys-file '" << BoneSysFile << "'");
			}
		}
		else
		{
			ERROR("Can't find device tree fragment '" << name << "'");
		}
	}

	if (loaded)
	{
		INFO("Loaded device tree fragment '" << name << "'");
	}

	return loaded;
}

bool CapeManager::unloadFragment(const std::string& name)
{
	bool unloaded = false;

	if (isFragmentLoaded(name))
	{
		std::ofstream out;

		out.open(BoneSysFile, std::ios::out | std::ios::app);

		if (out)
		{
			auto iter = m_fragments.find(name);

			TB_ASSERT(iter != m_fragments.end());

			out << "-" << boost::lexical_cast<std::string>(static_cast<int>(iter->second.m_slot)) << std::endl;
			out.close();

			unloaded = true;
		}
		else
		{
			ERROR("Can't open sys-file '" << BoneSysFile << "'");
		}
	}
	else
	{
		unloaded = true;
	}

	if (unloaded)
	{
		INFO("Unloaded device tree fragment '" << name << "'");
	}

	return unloaded;
}

bool CapeManager::isFragmentLoaded(const std::string& name)
{
	const FragmentMap& fragments = readFragments();

	return fragments.find(name) != fragments.end();
}

bool CapeManager::existsFragmentFile(const std::string& name) const
{
	const std::string regexp(name + ".*" + DefaultFragmentVersion + DefaultFragmentExtension);
	std::regex file_regex(regexp);

	for (boost::filesystem::directory_iterator itr(FragmentLibDir); itr != boost::filesystem::directory_iterator(); ++itr)
	{
		if (boost::filesystem::is_regular_file(itr->status()))
		{
			if (std::regex_match(itr->path().filename().string(), file_regex))
			{
				return true;
			}
		}
	}

	return false;
}

const CapeManager::FragmentMap& CapeManager::readFragments()
{
	m_fragments.clear();

	if (boost::filesystem::exists(BoneSysFile))
	{
		std::ifstream in(BoneSysFile);

		TB_ASSERT(in);

		std::string line;
		while (std::getline(in, line))
		{
			const Fragment fragment = parseLine(line);

			m_fragments[fragment.m_name] = std::move(fragment);
		}

		in.close();
	}

	return m_fragments;
}

CapeManager::Fragment CapeManager::parseLine(const std::string& line) const
{
	Fragment fragment;

	std::string splitStr = line;
	std::string token;

	if (Utils::split(splitStr, token, ":"))
	{
		// lexical_cast<uint8_t> not possible see http://www.boost.org/doc/libs/1_49_0/doc/html/boost_lexical_cast/frequently_asked_questions.html
		fragment.m_slot = boost::numeric_cast<uint8_t>(boost::lexical_cast<uint>(Utils::trim(token)));
	}

	if (Utils::split(splitStr, token, ":"))
	{
		fragment.m_i2cAddr = Utils::fromHexStr<uint8_t>(Utils::trim(token));
	}

	if (Utils::split(splitStr, token, " "))
	{
		fragment.m_flags = Utils::trim(token);
	}

	if (Utils::split(splitStr, token, ","))
	{
		fragment.m_boardName = Utils::trim(token);
	}

	if (Utils::split(splitStr, token, ","))
	{
		fragment.m_version = Utils::trim(token);
	}

	if (Utils::split(splitStr, token, ","))
	{
		fragment.m_manufacturer = Utils::trim(token);
	}

	if (splitStr.length() > 0)
	{
		fragment.m_name = Utils::trim(splitStr);
	}

	return fragment;
}
