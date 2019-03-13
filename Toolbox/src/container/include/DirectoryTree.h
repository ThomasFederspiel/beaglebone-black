/*
 * DirectoryTree.h
 *
 *  Created on: 4 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_DIRECTORYTREE_H_
#define TBOX_DIRECTORYTREE_H_

// standard
#include <chrono>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <queue>

// local
#include "IEnumerator.h"

// project
#include "exceptionMacros.h"
#include "tboxdefs.h"
#include "Utils.h"

namespace
{
static constexpr char DirSeparator = '/';
static const std::string DirSeparatorStr = "/";
}

namespace tbox
{
template <typename T>
class DirectoryTree final
{
private:
	class DirectoryNode;
	using DirectoryNodePtr = std::shared_ptr<DirectoryNode>;
	using DirectoryParentNodePtr = std::weak_ptr<DirectoryNode>;

	struct DirectoryNode
	{
	public:

		using DirectoryMap = std::map<std::string, DirectoryNodePtr>;
		using ItemMap = std::map<std::string, T>;

		explicit DirectoryNode(const std::string& name) : m_name(name), m_parent(), m_directories(), m_items()
		{
		}

		std::string m_name;
		DirectoryParentNodePtr m_parent;

		DirectoryMap m_directories;
		ItemMap m_items;
	};

public:

	enum class ErrorCode
	{
		Success = 0,
		RootSeparatorMissing,
		ItemNameMissing,
		PathMissing,
		PathNotFound,
		ItemNotFound,
		ItemExists
	};

	class DirectoryPath final
	{
	public:
		DirectoryPath() : DirectoryPath("")
		{
		}

		/**
		 * Path in format /xxx/yyy/<item name>
		 */
		explicit DirectoryPath(std::string path, const bool pathOnly = false) : m_directories(), m_item(), m_isRooted(false)
		{
			parsePath(Utils::trim(path), pathOnly);
		}

		bool empty() const
		{
			return !hasItem() && m_directories.empty();
		}

		bool isRooted() const
		{
			return m_isRooted;
		}

		bool hasItem() const
		{
			return !m_item.empty();
		}

		const std::string& getItem() const
		{
			return m_item;
		}

		std::string getLeaf() const
		{
			std::string leaf;

			if (!m_directories.empty())
			{
				leaf = m_directories.back();
			}

			return leaf;
		}

		std::string getBase() const
		{
			std::string base;

			if (!m_directories.empty())
			{
				base = m_directories.front();
			}

			return base;
		}

		std::string getPath() const
		{
			std::ostringstream oss;

			if (m_directories.empty())
			{
				oss << DirSeparator;
			}
			else
			{
				for (const auto& dir : m_directories)
				{
					oss << DirSeparator << dir;
				}
			}

			return oss.str();
		}

		const std::vector<std::string>& getDirectories() const
		{
			return m_directories;getDirectories();
		}

		void appendDirectory(const std::string& dir)
		{
			const auto path = getPath();

			m_directories.clear();

			const bool pathOnly = true;
			parsePath(Utils::trim(path + DirSeparator + dir, DirSeparator), pathOnly);
		}

		void stripLeafDirectory()
		{
			if (!m_directories.empty())
			{
				m_directories.pop_back();
			}
		}

	private:

		void parsePath(std::string path, const bool pathOnly)
		{
			std::string part;
			bool found = Utils::split(path, part, DirSeparatorStr);

			// Begun with /
			if (found && part.empty())
			{
				m_isRooted = true;

				found = Utils::split(path, part, DirSeparatorStr);
			}

			while (found)
			{
				m_directories.push_back(part);

				found = Utils::split(path, part, DirSeparatorStr);
			}

			if (pathOnly)
			{
				if (!path.empty())
				{
					m_directories.push_back(path);
				}
			}
			else
			{
				m_item = path;
			}
		}

		std::vector<std::string> m_directories;
		std::string m_item;
		bool m_isRooted;
	};

	class EnumeratorItem final
	{
	public:

		explicit EnumeratorItem() : m_path(), m_item(nullptr)
		{
		}

		explicit EnumeratorItem(const DirectoryPath path) : m_path(path), m_item(nullptr)
		{
		}

		explicit EnumeratorItem(const T& item) : m_path(), m_item(&item)
		{
		}

		bool hasItem() const
		{
			return m_item != nullptr;
		}

		const T& getItem() const
		{
			TB_ASSERT(hasItem());

			return *m_item;
		}

		bool hasPath() const
		{
			return !m_path.empty();
		}

		const DirectoryPath& getPath() const
		{
			TB_ASSERT(hasPath());

			return m_path;
		}

		const DirectoryPath m_path;
		const T* m_item;
	};

	class ItemEnumerator final : public IEnumerator<EnumeratorItem>
	{
	public:

		explicit ItemEnumerator(const DirectoryTree<T>& tree, const DirectoryPath& path, const bool recursive)
			: m_node(), m_stateStack(), m_state(path, tree.m_root), m_recursive(recursive), m_valid(false)
		{
			ErrorCode code = ErrorCode::Success;

			m_node = tree.getDirectoryNode(path, code);

			m_valid = code == ErrorCode::Success;

			m_state = State(path, m_node);
		}

		bool isValid() const
		{
			return m_valid;
		}

		bool hasMore() override
		{
			return (m_node != nullptr) && (m_state.hasDirectories() || m_state.hasItems() || m_stateStack.hasData());
		}

		EnumeratorItem next() override
		{
			TB_ASSERT(hasMore());

			if (m_recursive)
			{
				while (m_node)
				{
					if (m_state.hasItems())
					{
						auto item = EnumeratorItem(m_state.nextItem());

						return item;
					}

					while (m_state.hasDirectories())
					{
						m_node = m_state.nextDirectory();
						TB_ASSERT(m_node);

						m_stateStack.push(m_state);

						auto path = m_state.getPath();
						path.appendDirectory(m_node->m_name);
						m_state = State(path, m_node);

						if (m_state.hasItems())
						{
							auto item = EnumeratorItem(m_state.getPath());

							return item;
						}
					}

					TB_ASSERT(!m_stateStack.empty());

					m_node = m_node->m_parent.lock();

					TB_ASSERT(m_node != nullptr);

					m_state = m_stateStack.top();
					m_stateStack.pop();
				}

				TB_ERROR("Shouldn't be here");
			}
			else
			{
				if (m_state.hasDirectories())
				{
					constexpr bool pathOnly = true;
					return EnumeratorItem(DirectoryPath(m_state.nextDirectory()->m_name, pathOnly));
				}

				if (m_state.hasItems())
				{
					return EnumeratorItem(m_state.nextItem());
				}

				TB_ERROR("Shouldn't be here");
			}

			TB_ERROR("Shouldn't be here");

			return EnumeratorItem();
		}

	private:

		struct State final
		{
		public:
			explicit State(const DirectoryPath& path, DirectoryNodePtr node) : m_path(path), m_node(node)
			{
				if (m_node)
				{
					m_itemIter = node->m_items.begin();
					m_itemEnd = node->m_items.end();
					m_dirIter = node->m_directories.begin();
					m_dirEnd = node->m_directories.end();
				}
			}

			const DirectoryPath& getPath() const
			{
				return m_path;
			}

			bool hasDirectories() const
			{
				return m_node && (m_dirIter != m_dirEnd);
			}

			bool hasItems() const
			{
				return m_node && (m_itemIter != m_itemEnd);
			}

			DirectoryNodePtr nextDirectory()
			{
				TB_ASSERT(hasDirectories());

				DirectoryNodePtr node = m_dirIter->second;

				++m_dirIter;

				return node;
			}

			const T& nextItem()
			{
				TB_ASSERT(hasItems());

				const T& item = m_itemIter->second;

				++m_itemIter;

				return item;
			}

		private:
			DirectoryPath m_path;
			DirectoryNodePtr m_node;

			typename DirectoryNode::ItemMap::const_iterator m_itemIter;
			typename DirectoryNode::ItemMap::const_iterator m_itemEnd;

			typename DirectoryNode::DirectoryMap::const_iterator m_dirIter;
			typename DirectoryNode::DirectoryMap::const_iterator m_dirEnd;
		};

		struct StackState final
		{
		public:

			bool hasData() const
			{
				for (const auto& item : m_stackState)
				{
					if (item.hasItems() || item.hasDirectories())
					{
						return true;
					}
				}

				return false;
			}

			bool empty() const
			{
				return m_stackState.empty();
			}

			void push(const State& state)
			{
				m_stackState.push_back(state);
			}

			const State& top() const
			{
				return m_stackState.back();
			}

			void pop()
			{
				m_stackState.pop_back();
			}

		private:
			std::vector<State> m_stackState;
		};

		DirectoryNodePtr m_node;
		StackState m_stateStack;
		State m_state;
		bool m_recursive;
		bool m_valid;
	};

	friend class ItemEnumerator;

	DirectoryTree() : m_root(new DirectoryNode("root"))
	{
	}

	ItemEnumerator getEnumerator(const DirectoryPath&& path, const bool recursive = false) const
	{
		return ItemEnumerator(*this, path, recursive);
	}

	bool hasNode(const DirectoryPath& path) const
	{
		ErrorCode code = ErrorCode::Success;

		const auto node = getDirectoryNode(path, code);

		return node != nullptr;
	}

	bool insertItem(const std::string& path, T item, ErrorCode& code)
	{
		const DirectoryPath dirPath(path);

		return insertItem(dirPath, std::move(item), code);
	}

	bool insertItem(const DirectoryPath& path, T item, ErrorCode& code)
	{
		TB_ASSERT(item);

		bool inserted = false;

		code = ErrorCode::Success;

		auto node = getDirectoryNode(path, code);

		if (code != ErrorCode::RootSeparatorMissing)
		{
			if (node)
			{
				auto it = node->m_items.find(path.getItem());

				if (it == node->m_items.end())
				{
					node->m_items[path.getItem()] = std::move(item);

					code = ErrorCode::Success;

					inserted = true;
				}
				else
				{
					code = ErrorCode::ItemExists;
				}
			}
			else
			{
				node = createDirectoryNode(path);

				TB_ASSERT(node);

				node->m_items[path.getItem()] = std::move(item);

				code = ErrorCode::Success;

				inserted = true;
			}
		}

		return inserted;
	}

	bool deleteItem(const std::string& path, ErrorCode& code)
	{
		const DirectoryPath dirPath(path);

		return deleteItem(dirPath, code);
	}

	bool deleteItem(const DirectoryPath& path, ErrorCode& code)
	{
		code = ErrorCode::Success;

		if (!path.hasItem())
		{
			code = ErrorCode::ItemNameMissing;

			return false;
		}

		auto node = getDirectoryNode(path, code);

		if (node)
		{
			auto it = node->m_items.find(path.getItem());

			if (it != node->m_items.end())
			{
				node->m_items.erase(it);

				cleanupDirectorBranch(node);
			}
			else
			{
				code = ErrorCode::ItemNotFound;
			}
		}

		return code == ErrorCode::Success;
	}

	const T* getItem(const std::string& path, ErrorCode& code) const
	{
		const DirectoryPath dirPath(path);

		return getItem(dirPath, code);
	}

	const T* getItem(const DirectoryPath& path, ErrorCode& code) const
	{
		code = ErrorCode::Success;

		if (!path.hasItem())
		{
			code = ErrorCode::ItemNameMissing;
			return nullptr;
		}

		const auto node = getDirectoryNode(path, code);

		if (node)
		{
			auto it = node->m_items.find(path.getItem());

			if (it != node->m_items.end())
			{
				return &it->second;
			}
			else
			{
				code = ErrorCode::ItemNotFound;
			}
		}

		return nullptr;
	}

	static const char* toString(const ErrorCode code)
	{
		#define CASE(v) case v: return #v;
		switch (code)
		{
			CASE(ErrorCode::Success)
			CASE(ErrorCode::RootSeparatorMissing)
			CASE(ErrorCode::ItemNameMissing)
			CASE(ErrorCode::PathMissing)
			CASE(ErrorCode::PathNotFound)
			CASE(ErrorCode::ItemNotFound)
			CASE(ErrorCode::ItemExists)
		}
		static std::string tmp = std::to_string(static_cast<int>(code));
		return tmp.c_str();
		#undef CASE
	}


private:
	struct DirectoryItem
	{
	public:
		std::string m_name;
		T m_item;
	};

	void cleanupDirectorBranch(DirectoryNodePtr node)
	{
		while (node)
		{
			if (node->m_items.empty() && node->m_directories.empty())
			{
				auto parent = node->m_parent.lock();

				if (parent)
				{
					auto it = parent->m_directories.find(node->m_name);

					TB_ASSERT(it != parent->m_directories.end());

					parent->m_directories.erase(it);
				}

				node = parent;
			}
			else
			{
				node.reset();
			}
		}
	}

	DirectoryNodePtr getDirectoryNode(const DirectoryPath& path, ErrorCode& code) const
	{
		DirectoryNodePtr foundNode;

		if (!path.isRooted())
		{
			code = ErrorCode::RootSeparatorMissing;
			return foundNode;
		}

		DirectoryNodePtr node = m_root;

		std::size_t i = 0;

		const auto& dirNames = path.getDirectories();

		if (dirNames.empty())
		{
			code = ErrorCode::Success;
			foundNode = node;
		}
		else
		{
			while (!foundNode)
			{
				if (i == dirNames.size())
				{
					code = ErrorCode::Success;
					foundNode = node;
					break;
				}
				else
				{
					auto it = node->m_directories.find(dirNames.at(i));

					if (it != node->m_directories.end())
					{
						++i;
						node = it->second;
					}
					else
					{
						code = ErrorCode::PathNotFound;
						break;
					}
				}
			}
		}

		return foundNode;
	}

	DirectoryNodePtr createDirectoryNode(const DirectoryPath& path)
	{
		const auto& dirNames = path.getDirectories();

		auto node = m_root;

		for (const auto& name : dirNames)
		{
			auto it = node->m_directories.find(name);

			if (it == node->m_directories.end())
			{
				auto nextNode = std::make_shared<DirectoryNode>(name);

				node->m_directories[name] = nextNode;
				nextNode->m_parent = node;

				node = nextNode;
			}
			else
			{
				node = it->second;
			}
		}

		return node;
	}

	DirectoryNodePtr m_root;
};

} // namespace tbox

#endif /* TBOX_DIRECTORYTREE_H_ */
