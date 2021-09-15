#pragma once
#include <utility>
#include <vector>
#include "../Macro_Definitions/Definitions.h"

namespace ACBYTES
{
#pragma region Unique_Ptr
	template <typename T>
	class Unique_Ptr
	{
		T* _ptr;

	public:

		[[nodiscard]] Unique_Ptr(T* Ptr) : _ptr(Ptr)
		{
		}

		~Unique_Ptr()
		{
			if (_ptr)
				delete _ptr;
		}

		bool Valid()
		{
			return _ptr != nullptr;
		}

		T* Get()
		{
			return _ptr;
		}

		T* operator ->()
		{
			return _ptr;
		}

		Unique_Ptr(const Unique_Ptr&) = delete;
		Unique_Ptr& operator =(const Unique_Ptr&) = delete;
		Unique_Ptr(Unique_Ptr&&) noexcept
		{
		}
	};

	template <typename T>
	class Unique_Ptr<T[]>
	{
		T* _ptr;

	public:

		const size_t Size;

		[[nodiscard]] Unique_Ptr(T* ArrPtr, size_t _Size) : _ptr(ArrPtr), Size(_Size)
		{
		}

		~Unique_Ptr()
		{
			if (_ptr)
				delete[] _ptr;
		}

		bool Valid()
		{
			return _ptr != nullptr;
		}

		T* Get()
		{
			return _ptr;
		}

		T& operator [](size_t Index)
		{
			return *(_ptr + Index);
		}

		Unique_Ptr(const Unique_Ptr&) = delete;
		Unique_Ptr& operator =(const Unique_Ptr&) = delete;
		Unique_Ptr(Unique_Ptr&&) noexcept
		{
		}
	};

	/*
	* Makes a unique pointer to type T, constructed with its default constructor.
	* @param T [Type of the pointer]
	*/
	template <typename T>
	[[nodiscard]] Unique_Ptr<T> Make_Unique()
	{
		return Unique_Ptr<T>(new T());
	}

	/*
	* Makes a unique pointer to type T using T's constructor that takes ArgT.
	* @param T [Type of the pointer]
	* @param ArgT [Constructor argument types]
	*/
	template <typename T, typename... ArgT>
	[[nodiscard]] Unique_Ptr<T> Make_Unique(ArgT... Arguments)
	{
		auto _init = new T(std::forward<ArgT>(Arguments)...);
		return Unique_Ptr<T>(_init);
	}

	/*
	* Makes a unique pointer pointing to an array.
	* @param T [Type of the array]
	* @param First [First element of the array]
	* @param Rest [Rest of the elements]
	*/
	template <typename T, T First, T... Rest>
	[[nodiscard]] Unique_Ptr<T[]> Make_Unique()
	{
		T* _init = new T[]{ First, Rest... };
		size_t size = sizeof...(Rest) + 1;
		return Unique_Ptr<T[]>(_init, size);
	}
#pragma endregion Unique_Ptr

#pragma region Shared_Ptr

	struct Shared_Reference_Data final
	{
	private:
		uint32_t _count = 1;
		void* _ptr;

	public:

		Shared_Reference_Data(void* Ptr) : _ptr(Ptr)
		{
		}

		~Shared_Reference_Data()
		{
			if (_ptr && _count < 1)
				delete _ptr;
		}

		bool Dead()
		{
			return _count < 1;
		}

		bool operator ==(void* Ptr)
		{
			return Ptr == _ptr;
		}

		Shared_Reference_Data& operator ++()
		{
			_count++;
			return *this;
		}

		Shared_Reference_Data& operator --()
		{
			_count--;
			return *this;
		}

		Shared_Reference_Data() = delete;
	};

	struct Shared_Ptr_Container final
	{
	public:
		NO_DEFAULT_CONSTRUCTORS(Shared_Ptr_Container);
		template <typename> friend class Shared_Ptr;

	private:
		static std::vector<Shared_Reference_Data> references;

		static void AddNewReference(void* Ptr)
		{
			for (auto i = references.begin(); i != references.end(); i++)
			{
				if (*i == Ptr)
				{
					++(*i);
					return;
				}
			}
			references.push_back(Shared_Reference_Data(Ptr));
		}

		static void RemoveReference(void* Ptr)
		{
			for (auto i = references.begin(); i != references.end(); i++)
			{
				if (*i == Ptr)
				{
					if ((--(*i)).Dead())
						references.erase(i);
					return;
				}
			}
		}
	};

	std::vector<Shared_Reference_Data> Shared_Ptr_Container::references = std::vector<Shared_Reference_Data>();

	template <typename T>
	class Shared_Ptr
	{
		T* _ptr;
	public:
		[[nodiscard]] Shared_Ptr(T* Ptr) : _ptr(Ptr)
		{
			Shared_Ptr_Container::AddNewReference(Ptr);
		}

		Shared_Ptr(const Shared_Ptr& Ref)
		{
			Shared_Ptr_Container::AddNewReference(Ref._ptr);
			_ptr = Ref._ptr;
		}

		Shared_Ptr(Shared_Ptr&& Rvf) noexcept
		{
			Shared_Ptr_Container::AddNewReference(Rvf._ptr);
			_ptr = Rvf._ptr;
		}

		~Shared_Ptr()
		{
			Shared_Ptr_Container::RemoveReference(_ptr);
		}

		T* Get()
		{
			return _ptr;
		}

		T* operator ->()
		{
			return _ptr;
		}

		Shared_Ptr() = delete;
	};

	template <typename T>
	class Shared_Ptr<T[]>
	{
		T* _ptr;

	public:

		const size_t Size;

		[[nodiscard]] Shared_Ptr(T* Ptr, size_t _Size) : _ptr(Ptr), Size(_Size)
		{
			Shared_Ptr_Container::AddNewReference(Ptr);
		}

		Shared_Ptr(const Shared_Ptr& Ref)
		{
			Shared_Ptr_Container::AddNewReference(Ref._ptr);
			_ptr = Ref._ptr;
			Size = Ref.Size;
		}

		Shared_Ptr(Shared_Ptr&& Rvf) noexcept
		{
			Shared_Ptr_Container::AddNewReference(Rvf._ptr);
			_ptr = Rvf._ptr;
			Size = Rvf.Size;
		}

		~Shared_Ptr()
		{
			Shared_Ptr_Container::RemoveReference(_ptr);
		}

		T* Get()
		{
			return _ptr;
		}

		T& operator [](size_t Index)
		{
			return *(_ptr + Index);
		}

		Shared_Ptr() = delete;
	};

	/*
	* Makes a shared pointer to type T, constructed with its default constructor.
	* @param T [Type of the pointer]
	*/
	template <typename T>
	[[nodiscard]] Shared_Ptr<T> Make_Shared()
	{
		return Shared_Ptr<T>(new T());
	}

	/*
	* Makes a shared pointer to type T using T's constructor that takes ArgT.
	* @param T [Type of the pointer]
	* @param ArgT [Constructor argument types]
	*/
	template <typename T, typename... ArgT>
	[[nodiscard]] Shared_Ptr<T> Make_Shared(ArgT... Arguments)
	{
		auto _init = new T(std::forward<ArgT>(Arguments)...);
		return Shared_Ptr<T>(_init);
	}

	/*
	* Makes a shared pointer pointing to an array.
	* @param T [Type of the array]
	* @param First [First element of the array]
	* @param Rest [Rest of the elements]
	*/
	template <typename T, T First, T... Rest>
	[[nodiscard]] Shared_Ptr<T[]> Make_Shared()
	{
		T* _init = new T[]{ First, Rest... };
		size_t size = sizeof...(Rest) + 1;
		return Shared_Ptr<T[]>(_init, size);
	}
#pragma endregion Shared_Ptr
}