#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <atomic>

template <class Type>
class Singleton
{
public:
	using CountType = std::atomic<uint32_t>;
	using UnderlyingType = Type*;

private:
	static CountType s_count;
	static UnderlyingType s_instance;

public:
	static void Destroy()
	{
		if (s_count > 0)
		{
			--s_count;
			if (s_count == 0)
			{
				delete s_instance;
				s_instance = nullptr;
			}
		}
	}

	static Type* GetInstance()
	{
		assert(s_instance != nullptr);
		return s_instance;
	}

	static void Initialize()
	{
		if (s_instance == nullptr)
			s_instance = new Type();

		++s_count;
	}
};

#endif // SINGLETON_HPP
