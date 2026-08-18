#pragma once
#include "utility.h"
#include <atomic>

BEGIN_GAIA

#define GAIA_REF_COUNT_IMPL \
		int32_t m_refCount;\
	public:\
		long_t addRef(){return ++m_refCount;}\
		long_t release(){long_t refCount = --m_refCount; if(0 == refCount) delete this; return refCount;}

#define GAIA_ATOMIC_REF_COUNT_IMPL \
		std::atomic<int32_t> m_refCount;\
	public:\
		int32_t addRef(){return ++m_refCount;}\
		int32_t release(){int32_t refCount = --m_refCount; if(0 == refCount) delete this; return refCount;}


template<typename T>
class RefCountImpl : public T
{
public:
	using T::T;
public:
	int32_t incRef()
	{
		return ++m_refCount;
	}
	int32_t decRef()
	{
		int32_t res = --m_refCount;
		if (0 == res)
		{
			delete this;
		}
		return res;
	}
private:
	int32_t m_refCount{ 0 };
};

template<typename T>
class AtomicRefCountImpl : public T
{
public:
	using T::T;
public:
	int32_t incRef()
	{
		return ++m_refCount;
	}
	int32_t decRef()
	{
		int32_t res = --m_refCount;
		if (0 == res)
		{
			delete this;
		}
		return res;
	}
private:
	std::atomic<int32_t> m_refCount{ 0 };
};

template<typename T>
class RefPtr
{
public:
	RefPtr() = default;

	explicit RefPtr(T* ptr) noexcept : 
		m_ptr(ptr)
	{
		if (m_ptr)
		{
			m_ptr->incRef();
		}
	}

	RefPtr(RefPtr&& other) noexcept
	{
		m_ptr = other.m_ptr;
		other.m_ptr = nullptr;
	}

	RefPtr(const RefPtr& other) noexcept : 
		m_ptr(other.m_ptr)
	{
		if (m_ptr)
		{
			m_ptr->incRef();
		}
	}

	~RefPtr()
	{
		if (m_ptr)
		{
			m_ptr->decRef();
		}
	}

	RefPtr& operator=(RefPtr&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ptr)
			{
				m_ptr->decRef();
			}
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
		}
		return *this;
	}

	RefPtr& operator=(const RefPtr& other) noexcept
	{
		assign(other.m_ptr);
		return *this;
	}

	RefPtr& operator=(const T* ptr) noexcept
	{
		assign(ptr);
		return *this;
	}
public:
	void assign(T* ptr) noexcept
	{
		if (m_ptr != ptr)
		{
			if (ptr)
			{
				ptr->incRef();
			}
			if (m_ptr)
			{
				m_ptr->decRef();
			}
			m_ptr = ptr;
		}
	}

	T* get() const noexcept
	{
		return m_ptr;
	}

	operator T* () noexcept
	{
		return m_ptr;
	}

	operator const T* () const noexcept
	{
		return m_ptr;
	}

	T& operator*() noexcept
	{
		return *m_ptr;
	}

	const T& operator*() const noexcept
	{
		return *m_ptr;
	}

	T* operator->() noexcept
	{
		return m_ptr;
	}

	const T* operator->() const noexcept
	{
		return m_ptr;
	}

	bool operator == (const RefPtr& arg) const noexcept
	{
		return m_ptr == arg.m_ptr;
	}

	bool operator == (RefPtr& arg) const noexcept
	{
		return m_ptr == arg.m_ptr;
	}

	bool operator != (const RefPtr& arg) const noexcept
	{
		return m_ptr != arg.m_ptr;
	}

	bool operator != (RefPtr& arg) const noexcept
	{
		return m_ptr != arg.m_ptr;
	}

	bool operator == (const T* p) const noexcept
	{
		return (m_ptr == p);
	}

	bool operator == (T* p) const noexcept
	{
		return (m_ptr == p);
	}

	bool operator != (const T* p) const noexcept
	{
		return (m_ptr != p);
	}

	bool operator != (T* p) const noexcept
	{
		return (m_ptr != p);
	}

	operator bool() const noexcept
	{
		return 0 != m_ptr;
	}

	bool operator !() const noexcept
	{
		return 0 == m_ptr;
	}

private:
	T* m_ptr{ nullptr };
};


END_GAIA