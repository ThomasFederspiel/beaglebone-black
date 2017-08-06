/*
 * AbstractCUICommand.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_ABSTRACTCUICOMMAND_H_
#define TBOX_ABSTRACTCUICOMMAND_H_

// standard
#include <string>

// local
#include "ICUICommand.h"

// project
#include "Utils.h"

class AbstractCUICommand : public ICUICommand
{
public:

	explicit AbstractCUICommand(const std::string& name, const std::string& path) : m_name(name), m_path(path)
	{
	}

	std::string getPath() const override
	{
		return Utils::concatPath(m_path, m_name);
	}

	const std::string& getName() const override
	{
		return m_name;
	}

	std::string shortDescription() const override
	{
		std::ostringstream ss;

		ss << m_name << " - ";

		genShortDesc(ss);

		return ss.str();
	}

	std::string manPage() const override
	{
		std::ostringstream ss;

		ss << m_name << " - ";

		genShortDesc(ss);
		genManPage(ss);

		return ss.str();
	}

protected:
	virtual void genShortDesc(std::ostream& stream) const = 0;

	virtual void genManPage(std::ostream& stream) const
	{
	}

private:
	const std::string m_name;
	const std::string m_path;
};

#endif /* TBOX_ABSTRACTCUICOMMAND_H_ */
