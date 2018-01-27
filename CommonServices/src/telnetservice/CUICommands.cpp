/*
 * CUICommands.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "CUICommands.h"

// standard
#include <ostream>

// project
#include "CUICommandContextImpl.h"
#include "CUICommandContext.h"
#include "CUICommandParser.h"
#include "ICUICommandParser.h"
#include "Logger.h"
#include "Stringifyer.h"
#include "Utils.h"

MODULE_LOG(CUICommands);

namespace
{
static const std::string CdParentDir = "..";
static const std::string CommandPath = "/";

CUICommandContextImpl::DirectoryTree::DirectoryPath makeRootedPath(const std::string& path, CUICommandContextImpl& context)
{
	constexpr bool pathOnly = true;
	CUICommandContextImpl::DirectoryTree::DirectoryPath treePath(path, pathOnly);

	if (!treePath.isRooted())
	{
		treePath = CUICommandContextImpl::DirectoryTree::DirectoryPath(context.getCWD(), pathOnly);
		treePath.appendDirectory(path);
	}

	return treePath;
}
}

AbstractSystemCUICommand::AbstractSystemCUICommand(const std::string& name, const std::string& path)
	: AbstractCUICommand(name, path)
{
}

void AbstractSystemCUICommand::invoke(const ICUICommandParser& commandLine, CUICommandContext& context)
{
	invoke(commandLine, dynamic_cast<CUICommandContextImpl&>(context.getImpl()));
}

PwdCommand::PwdCommand() : AbstractSystemCUICommand("pwd", CommandPath)
{
}

void PwdCommand::invoke(const ICUICommandParser& commandParser, CUICommandContextImpl& context)
{
	context.output() << context.getCWD();

	context.finalize();
}

void PwdCommand::genShortDesc(std::ostream& stream) const
{
	stream << "show current command level";
}

CdCommand::CdCommand() : AbstractSystemCUICommand("cd", CommandPath)
{
}

void CdCommand::invoke(const ICUICommandParser& commandParser, CUICommandContextImpl& context)
{
	if (commandParser.count() > 1)
	{
		context.finalizeError("Too many arguments");
		return;
	}

	if (commandParser.count() == 1)
	{
		std::string arg1;
		if (commandParser.getStringArgument(0, arg1))
		{
			if (arg1 == CdParentDir)
			{
				constexpr bool pathOnly = true;
				CUICommandContextImpl::DirectoryTree::DirectoryPath path(context.getCWD(), pathOnly);
				path.stripLeafDirectory();

				arg1 = path.getPath();
			}

			const auto path = makeRootedPath(arg1, context);

			const auto& tree = context.getTree();

			if (tree.hasNode(path))
			{
				context.setCWD(path.getPath());
				context.finalize();
			}
			else
			{
				context.finalizeError(TBOX_STRINGIFY("Path " << arg1 << " doesn't exist"));
			}
		}
		else
		{
			context.finalizeError("Bad path specified");
		}
	}
	else
	{
		context.finalizeError("Bad command, missing path");
	}
}

void CdCommand::genShortDesc(std::ostream& stream) const
{
	stream << "change current command level";
}

LsCommand::LsCommand() : AbstractSystemCUICommand("ls", CommandPath)
{
}

void LsCommand::invoke(const ICUICommandParser& commandParser, CUICommandContextImpl& context)
{
	if (commandParser.count() > 2)
	{
		context.finalizeError("Too many arguments");
		return;
	}

	std::string path = context.getCWD();

	std::string arg1;
	if (commandParser.getStringArgument(0, arg1))
	{
		path = arg1;
	}

	const bool recursive = commandParser.hasOption('r');
	auto enumerator = context.getTree().getEnumerator(makeRootedPath(path, context), recursive);

	if (!enumerator.isValid())
	{
		context.finalizeError(TBOX_STRINGIFY("Path " << path << " doesn't exist"));
		return;
	}

	const bool shortDesc = commandParser.hasOption('s');
	const bool manPage = commandParser.hasOption('m');

	while (enumerator.hasMore())
	{
		const auto& elem = enumerator.next();

		if (elem.hasItem())
		{
			const auto& item = elem.getItem();

			std::string desc = item->getName();

			if (manPage)
			{
				desc = item->manPage();
			}
			else if (shortDesc)
			{
				desc = item->shortDescription();
			}

			context.output() << desc << context.newline();
		}

		if (elem.hasPath())
		{
			if (recursive)
			{
				context.output() << context.newline();
				context.output() << elem.getPath().getPath() << context.newline();
				context.output() << context.newline();
			}
			else
			{
				context.output() << "<" << elem.getPath().getLeaf() << ">" << context.newline();
			}
		}
	}

	context.finalize();
}

void LsCommand::genShortDesc(std::ostream& stream) const
{
	stream << "list commands";
}

QuitCommand::QuitCommand() : AbstractSystemCUICommand("quit", CommandPath)
{
}

void QuitCommand::invoke(const ICUICommandParser& commandParser, CUICommandContextImpl& context)
{
	context.shutdown();
}

void QuitCommand::genShortDesc(std::ostream& stream) const
{
	stream << "closes connection";
}

