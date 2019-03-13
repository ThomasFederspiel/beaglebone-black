/*
 * CapeManager.h
 *
 *  Created on: 26 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_CAPEMANAGER_H_
#define TBOX_CAPEMANAGER_H_

#include <string>
#include <stdint.h>
#include <map>

class CapeManager final
{
public:

	struct Fragment final
	{
	public:

		using fragmentName_t = std::string;

		Fragment() :
			m_slot(0), m_i2cAddr(0), m_flags(), m_boardName(), m_version(), m_manufacturer(), m_name()
		{
		}

		uint8_t m_slot;
		uint8_t m_i2cAddr;
		std::string m_flags;
		std::string m_boardName;
		std::string m_version;
		std::string m_manufacturer;
		fragmentName_t m_name;
	};

	static bool isSupported();

	bool loadFragment(const std::string& name);
	bool unloadFragment(const std::string& name);
	bool isFragmentLoaded(const std::string& name);
	bool existsFragmentFile(const std::string& name) const;
private:

	using FragmentMap = std::map<Fragment::fragmentName_t, Fragment>;

	const FragmentMap& readFragments();
	Fragment parseLine(const std::string& line) const;

	FragmentMap m_fragments;
};

#endif /* TBOX_CAPEMANAGER_H_ */
