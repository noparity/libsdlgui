#ifndef FLAGS_HPP
#define FLAGS_HPP

template <typename T>
class Flags
{
private:
	typename std::underlying_type<T>::type m_flags;

public:
	Flags() : m_flags(0) {}
	Flags(T flags) : m_flags(static_cast<std::underlying_type<T>::type>(flags)) {}

	T Get() const { return static_cast<T>(m_flags); }
	T operator&(T rhs) const { return static_cast<T>(m_flags & rhs); }
	Flags& operator|=(T rhs)
	{
		m_flags |= static_cast<std::underlying_type<T>::type>(rhs);
		return *this;
	}

	Flags& operator^=(T rhs)
	{
		m_flags = m_flags & ~static_cast<std::underlying_type<T>::type>(rhs);
		return *this;
	}

	// for iterating over the bits that are set
	class Iterator
	{
	private:
		friend Iterator Flags::Begin() const;
		friend Iterator Flags::End() const;

		typename std::underlying_type<T>::type m_flags;
		typename std::underlying_type<T>::type m_pos;

		Iterator(typename std::underlying_type<T>::type flags, typename std::underlying_type<T>::type pos) : m_flags(flags), m_pos(pos)
		{
			MoveNext(false);
		}

		void MoveNext(bool advance)
		{
			while (((m_flags & m_pos) == 0 || advance) && static_cast<T>(m_pos) != T::END)
			{
				m_pos <<= 1;
				advance = false;
			}
		}

	public:
		bool operator==(const Iterator& rhs)
		{
			return m_pos == rhs.m_pos;
		}

		bool operator!=(const Iterator& rhs)
		{
			return !(*this == rhs);
		}

		Iterator& operator++()
		{
			MoveNext(true);
			return *this;
		}

		T operator*()
		{
			return static_cast<T>(m_flags & m_pos);
		}
	};

	Iterator Begin() const
	{
		return Iterator(m_flags, 1);
	}

	Iterator End() const
	{
		return Iterator(m_flags, static_cast<std::underlying_type<T>::type>(T::END));
	}
};

#endif FLAGS_HPP
