/*
 * IComConnection.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_ICOMCONNECTION_H_
#define TBOX_ICOMCONNECTION_H_

// standard
#include <cstdint>
#include <memory>

class IComConnection
{
public:
	using ConnectionPtr = std::shared_ptr<IComConnection>;
	using IDType = std::size_t;

	class ComBuffer
	{
	public:

		using Data = std::vector<uint8_t>;

		explicit ComBuffer(const std::size_t size) : m_data(size)
		{
			m_contentSize = 0;
		}

		explicit ComBuffer(const std::string& data) : m_data(data.length())
		{
			m_data.assign(data.begin(), data.end());
			m_contentSize = data.size();
		}

		explicit ComBuffer(const Data& data) : m_data(data.size())
		{
			m_data.assign(data.begin(), data.end());
			m_contentSize = data.size();
		}

		void clear()
		{
			m_data.clear();
		}

		bool empty() const
		{
			return m_data.empty();
		}

		void erase(Data::iterator first, Data::iterator last)
		{
			m_data.erase(first, last);
		}

		void setContentSize(const std::size_t size)
		{
			m_contentSize = size;
		}

		std::size_t contentSize() const
		{
			return m_contentSize;
		}

		std::size_t capacity() const
		{
			return m_data.size();
		}

		Data& data()
		{
			return m_data;
		}

		const Data& data() const
		{
			return m_data;
		}

		uint8_t operator[](const std::size_t index)
		{
			return m_data.at(index);
		}

		Data::iterator begin()
		{
			return m_data.begin();
		}

		Data::iterator end()
		{
			return m_data.end();
		}

		Data::const_iterator begin() const
		{
			return m_data.begin();
		}

		Data::const_iterator end() const
		{
			return m_data.end();
		}

	private:
		Data m_data;
		std::size_t m_contentSize;
	};

	virtual ~IComConnection()
	{
	}

	virtual IDType getId() const = 0;
	virtual ComBuffer& getRxBuffer() = 0;
	virtual bool isOpen() const = 0;

	// Thread safe
	virtual void close() = 0;

	// Thread safe
	virtual void send(const std::string& data) = 0;
	virtual void send(const std::vector<uint8_t>& data) = 0;
};

#endif /* TBOX_ICOMCONNECTION_H_ */
