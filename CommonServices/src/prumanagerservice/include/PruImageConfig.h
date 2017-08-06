/*
 * PruImageConfig.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef PRUIMAGECONFIG_H_
#define PRUIMAGECONFIG_H_

// standard
#include <string>

// local

// project

class PruImageConfig final
{
public:
	enum class PruId
	{
		Pru0 = 0,
		Pru1
	};

	explicit PruImageConfig(const PruId pru, const std::string& textBin, const std::string& dataBin,
			const std::size_t shramSize, const std::size_t ddrSize)
		: m_pru(pru), m_textBin(textBin), m_dataBin(dataBin), m_shramSize(shramSize), m_ddrSize(ddrSize)
	{
	}

	PruId getPru() const
	{
		return m_pru;
	}

	const std::string& getTextBin() const
	{
		return m_textBin;
	}

	const std::string& getDataBin() const
	{
		return m_dataBin;
	}

	std::size_t getDDRSize() const
	{
		return m_ddrSize;
	}

	std::size_t getShramSize() const
	{
		return m_shramSize;
	}

	static const char* toString(const PruId id)
	{
		#define CASE(v) case v: return #v;
		switch (static_cast<PruId>(id))
		{
			CASE(PruId::Pru0)
			CASE(PruId::Pru1)
		}
		static std::string tmp = std::to_string(static_cast<int>(id));
		return tmp.c_str();
		#undef CASE
	}

private:

	PruId m_pru;
	std::string m_textBin;
	std::string m_dataBin;

	std::size_t m_shramSize;
	std::size_t m_ddrSize;
};

#endif /* PRUIMAGECONFIG_H_ */
