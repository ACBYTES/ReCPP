#pragma once

#include <initializer_list>
#include <vector>
#include "Definitions.h"
#include "Type_Traits.h"

namespace ACBYTES
{
#pragma region Unique_Ptr
	template <typename T>
	class Unique_Ptr
	{
		T* _ptr = nullptr;

	public:

		[[nodiscard]] Unique_Ptr(std::nullptr_t = nullptr) //Empty pointer.
		{
		}

		[[nodiscard]] Unique_Ptr(T* Ptr) : _ptr(Ptr)
		{
		}

		[[nodiscard]] Unique_Ptr(Unique_Ptr&& Rvr) noexcept
		{
			_ptr = Rvr._ptr;
			Rvr._ptr = nullptr;
		}

		~Unique_Ptr()
		{
			if (_ptr)
				delete _ptr;
		}

		void Swap(Unique_Ptr& Ref)
		{
			auto ptr = _ptr;
			_ptr = Ref._ptr;
			Ref._ptr = ptr;
		}

		void Release()
		{
			_ptr = nullptr;
		}

		void Reset(T* Ptr = nullptr)
		{
			if (_ptr)
				delete _ptr;
			_ptr = Ptr;
		}

		bool Valid() const
		{
			return _ptr != nullptr;
		}

		T* Get() const
		{
			return _ptr;
		}

		T* operator ->()
		{
			return _ptr;
		}

		Unique_Ptr(const Unique_Ptr&) = delete;
		Unique_Ptr& operator =(const Unique_Ptr&) = delete;
	};

	template <typename T>
	class Unique_Ptr<T[]>
	{
		T* _ptr = nullptr;
		size_t size;

	public:

		[[nodiscard]] Unique_Ptr(std::nullptr_t = nullptr) //Empty pointer.
		{
		}

		[[nodiscard]] Unique_Ptr(T* ArrPtr, size_t Size) : _ptr(ArrPtr), size(Size)
		{
		}

		[[nodiscard]] Unique_Ptr(Unique_Ptr&& Rvr) noexcept
		{
			_ptr = Rvr._ptr;
			size = Rvr.size;
			Rvr._ptr = nullptr;
			Rvr.size = 0;
		}

		~Unique_Ptr()
		{
			if (_ptr)
				delete[] _ptr;
		}

		void Swap(Unique_Ptr& Ref)
		{
			auto ptr = _ptr;
			auto _size = size;
			_ptr = Ref._ptr;
			size = Ref.size;
			Ref._ptr = ptr;
			Ref.size = _size;
		}

		void Release()
		{
			_ptr = nullptr;
		}

		void Reset(T* Ptr, size_t Size)
		{
			if (_ptr)
				delete[] _ptr;
			_ptr = Ptr;
			size = Size;
		}

		void Reset(T* Ptr = nullptr) //Unable to resolve size
		{
			if (_ptr)
				delete[] _ptr;
			_ptr = Ptr;
			size = size_t();
		}

		bool Valid() const
		{
			return _ptr != nullptr;
		}

		size_t Size() const
		{
			return size;
		}

		T* Get() const
		{
			return _ptr;
		}

		template <size_t ArrSize>
		void Fill(T(&Array)[ArrSize]) const
		{
			for (size_t i = 0; i < ArrSize > size ? size : ArrSize; i++)
			{
				Array[i] = *(_ptr + i);
			}
		}

		T& operator [](size_t Index)
		{
			return *(_ptr + Index);
		}

		Unique_Ptr(const Unique_Ptr&) = delete;
		Unique_Ptr& operator =(const Unique_Ptr&) = delete;
	};

	/*
	* Makes unique pointer pointing to an array with the size passed.
	*/
	template <typename T, enable_if_t<is_array<T>::value, bool> = false>
	[[nodiscard]] auto Make_Unique(const size_t Size) -> Unique_Ptr<T>
	{
		using type = remove_array_t<T>;
		type* _init = new type[Size]{}; //Default constructed for possible const types.
		return Unique_Ptr<T>(_init, Size);
	}

	/*
	* Makes unique pointer pointing to an array initialized with the initializer list passed.
	*/
	template <typename T, enable_if_t<is_array<T>::value && !is_const<T>::value, bool> = false> //No const types allowed because the array is going to be filled with the initializer list.
	[[nodiscard]] auto Make_Unique(std::initializer_list<remove_array_t<T>> List) -> Unique_Ptr<T>
	{
		using type = remove_array_t<T>;
		type* _init = new type[List.size()];
		for (size_t i = 0; i < List.size(); i++)
		{
			_init[i] = *(List.begin() + i);
		}
		return Unique_Ptr<T>(_init, List.size());
	}

	/*
	* Makes unique pointer pointing to an object of type T.
	*/
	template <typename T, typename... ArgT, enable_if_t<!is_array<T>::value, bool> = false>
	[[nodiscard]] auto Make_Unique(ArgT... Arguments) -> Unique_Ptr<T>
	{
		auto _init = new T(Forward<ArgT>(Arguments)...);
		return Unique_Ptr<T>(_init);
	}
#pragma endregion Unique_Ptr

#pragma region Shared_Ptr
	struct Shared_Ref_Counter final
	{
	private:
		uint32_t _count = 1;
		void* _ptr = nullptr;
		bool _array;

	public:

		Shared_Ref_Counter(void* Ptr, bool IsArray) : _ptr(Ptr), _array(IsArray)
		{
		}

		~Shared_Ref_Counter()
		{
			if (_ptr && _count < 1)
			{
				if (_array)
					delete[] _ptr; //@TODO: FIX RUNTIME EXCEPTION. [UNKNOWN BASE TYPE - VOID*]
				else
					delete _ptr;
			}
		}

		bool Dead()
		{
			return _count < 1;
		}

		//bool operator ==(void* Ptr)
		//{
		//	return Ptr == _ptr;
		//}

		template <typename T>
		bool operator ==(T Ptr) //Types might contain different qualifiers. [Could be simplified to the version above where casts to void* are done in Shared_Ptr_Container functions themselves.]. This is surely (At least until no users use this class [If used, handling can happen using enable_if or explicit array type using remove_array*]) gets called from Shared_Ptr_Container so checks for correct pointer types isn't necessary.
		{
			return (void*)Ptr == _ptr;
		}

		Shared_Ref_Counter& operator ++()
		{
			_count++;
			return *this;
		}

		Shared_Ref_Counter& operator --()
		{
			_count--;
			return *this;
		}

		Shared_Ref_Counter() = delete;
	};

	struct Shared_Ptr_Container final
	{
	public:
		NO_DEFAULT_CONSTRUCTORS(Shared_Ptr_Container);
		template <typename> friend class Shared_Ptr;

	private:
		static std::vector<Shared_Ref_Counter> references;
		static const size_t reserve_size = 4;

		static void CheckReserve()
		{
			references.reserve(references.capacity() - references.size() <= 2 ? reserve_size : 0);
		}

		template <typename T>
		static void AddNewReference(T* Ptr, bool IsArray = false)
		{
			if (Ptr)
			{
				CheckReserve();
				for (auto i = references.begin(); i != references.end(); i++)
				{
					if (*i == Ptr)
					{
						++(*i);
						return;
					}
				}
				references.push_back(Shared_Ref_Counter((void*)Ptr, IsArray));
			}
		}

		template <typename T>
		static void RemoveReference(T* Ptr)
		{
			if (Ptr)
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
		}
	};

	std::vector<Shared_Ref_Counter> Shared_Ptr_Container::references = std::vector<Shared_Ref_Counter>();

	template <typename T>
	class Shared_Ptr
	{
		T* _ptr = nullptr;

	public:

		[[nodiscard]] Shared_Ptr(std::nullptr_t = nullptr) //Empty pointer.
		{
		}

		[[nodiscard]] Shared_Ptr(T* Ptr) : _ptr(Ptr)
		{
			Shared_Ptr_Container::AddNewReference(Ptr);
		}

		Shared_Ptr(const Shared_Ptr& Ref)
		{
			_ptr = Ref._ptr;
			Shared_Ptr_Container::AddNewReference(_ptr);
		}

		[[nodiscard]] Shared_Ptr(Shared_Ptr&& Rvr) noexcept
		{
			_ptr = Rvr._ptr;
			Rvr._ptr = nullptr;
		}

		template <typename T1, enable_if_t<is_base_of<T, T1>::value, bool> = false>
		[[nodiscard]] Shared_Ptr(const Shared_Ptr<T1>& Ref)
		{
			_ptr = Ref.Get();
			Shared_Ptr_Container::AddNewReference(_ptr);
		}

		~Shared_Ptr()
		{
			Shared_Ptr_Container::RemoveReference(_ptr);
		}

		void Swap(Shared_Ptr& Ref)
		{
			auto ptr = _ptr;
			_ptr = Ref._ptr;
			Ref._ptr = ptr;
		}

		void Reset(T* Ptr = nullptr)
		{
			Shared_Ptr_Container::RemoveReference(_ptr);
			Shared_Ptr_Container::AddNewReference(Ptr);

			_ptr = Ptr;
		}

		T* Get() const
		{
			return _ptr;
		}

		T* operator ->()
		{
			return _ptr;
		}
	};

	template <typename T>
	class Shared_Ptr<T[]>
	{
		T* _ptr = nullptr;
		size_t size;

	public:

		[[nodiscard]] Shared_Ptr(std::nullptr_t = nullptr) //Empty pointer.
		{
		}

		[[nodiscard]] Shared_Ptr(T* Ptr, size_t Size) : _ptr(Ptr), size(Size)
		{
			Shared_Ptr_Container::AddNewReference(Ptr, true);
		}

		Shared_Ptr(const Shared_Ptr& Ref)
		{
			_ptr = Ref._ptr;
			size = Ref.Size();
			Shared_Ptr_Container::AddNewReference(Ref._ptr, true);
		}

		[[nodiscard]] Shared_Ptr(Shared_Ptr&& Rvr) noexcept
		{
			_ptr = Rvr._ptr;
			size = Rvr.Size();
			Rvr._ptr = nullptr;
			Rvr.size = 0;
		}

		template <typename T1, enable_if_t<is_base_of<remove_array_t<T>, remove_array_t<T1>>::value, bool> = false>
		[[nodiscard]] Shared_Ptr(const Shared_Ptr<T1>& Ref)
		{
			_ptr = Ref.Get();
			size = Ref.Size();
			Shared_Ptr_Container::AddNewReference(_ptr, true);
		}

		~Shared_Ptr()
		{
			Shared_Ptr_Container::RemoveReference(_ptr);
		}

		void Swap(Shared_Ptr& Ref)
		{
			auto ptr = _ptr;
			auto _size = size;
			_ptr = Ref._ptr;
			size = Ref.size;
			Ref._ptr = ptr;
			Ref.size = _size;
		}

		void Reset(T* Ptr, size_t Size)
		{
			Shared_Ptr_Container::RemoveReference(_ptr);
			Shared_Ptr_Container::AddNewReference(Ptr);

			_ptr = Ptr;
			size = Size;
		}

		void Reset(T* Ptr = nullptr) //Unable to resolve size
		{
			Shared_Ptr_Container::RemoveReference(_ptr);
			Shared_Ptr_Container::AddNewReference(Ptr);

			_ptr = Ptr;
			size = size_t();
		}

		size_t Size() const
		{
			return size;
		}

		T* Get() const
		{
			return _ptr;
		}

		template <size_t ArrSize>
		void Fill(T(&Array)[ArrSize]) const
		{
			for (size_t i = 0; i < ArrSize > size ? size : ArrSize; i++)
			{
				Array[i] = *(_ptr + i);
			}
		}

		T& operator [](size_t Index)
		{
			return *(_ptr + Index);
		}
	};

	/*
	* Makes shared pointer pointing to an array with the size passed.
	*/
	template <typename T, enable_if_t<is_array<T>::value, bool> = false>
	[[nodiscard]] auto Make_Shared(const size_t Size) -> Shared_Ptr<T>
	{
		using type = remove_array_t<T>;
		type* _init = new type[Size]{}; //Default constructed for possible const types.
		return Shared_Ptr<T>(_init, Size);
	}

	/*
	* Makes shared pointer pointing to an array initialized with the initializer list passed.
	*/
	template <typename T, enable_if_t<is_array<T>::value && !is_const<T>::value, bool> = false> //No const types allowed because the array is going to be filled with the initializer list.
	[[nodiscard]] auto Make_Shared(std::initializer_list<remove_array_t<T>> List) -> Shared_Ptr<T>
	{
		using type = remove_array_t<T>;
		type* _init = new type[List.size()];
		for (size_t i = 0; i < List.size(); i++)
		{
			_init[i] = *(List.begin() + i);
		}
		return Shared_Ptr<T>(_init, List.size());
	}

	/*
	* Makes shared pointer pointing to an object of type T.
	*/
	template <typename T, typename... ArgT, enable_if_t<!is_array<T>::value, bool> = false>
	[[nodiscard]] auto Make_Shared(ArgT... Arguments) -> Shared_Ptr<T>
	{
		auto _init = new T(Forward<ArgT>(Arguments)...);
		return Shared_Ptr<T>(_init);
	}
#pragma endregion Shared_Ptr

#pragma region Weak_Ptr
	template <typename T>
	class Weak_Ptr
	{
		T* _ptr = nullptr;

	public:
		Weak_Ptr(const Shared_Ptr<T>& Ref)
		{
			_ptr = Ref.Get();
		}

		Weak_Ptr() //Empty pointer
		{
		}

		[[nodiscard]] Weak_Ptr(Weak_Ptr&& Rvr) noexcept
		{
			_ptr = Rvr._ptr;
			Rvr._ptr = nullptr;
		}

		[[nodiscard]] Shared_Ptr<T> Lock() const
		{
			return _ptr ? Shared_Ptr<T>(_ptr) : Shared_Ptr<T>();
		}

		void Swap(Weak_Ptr& Ref)
		{
			auto ptr = _ptr;
			_ptr = Ref._ptr;
			Ref._ptr = ptr;
		}

		void Reset()
		{
			_ptr = nullptr;
		}

		T* Get() const
		{
			return _ptr;
		}

		T* operator ->()
		{
			return _ptr;
		}
	};

	template <typename T>
	class Weak_Ptr<T[]>
	{
		T* _ptr = nullptr;
		size_t size;

	public:

		Weak_Ptr(const Shared_Ptr<T[]>& Ref)
		{
			_ptr = Ref.Get();
			size = Ref.Size();
		}

		Weak_Ptr() //Empty pointer
		{
		}

		[[nodiscard]] Weak_Ptr(Weak_Ptr&& Rvr) noexcept
		{
			_ptr = Rvr._ptr;
			size = Rvr.size;
			Rvr._ptr = nullptr;
			Rvr.size = 0;
		}

		[[nodiscard]] Shared_Ptr<T[]> Lock() const
		{
			return _ptr ? Shared_Ptr<T[]>(_ptr, size) : Shared_Ptr<T[]>();
		}

		void Swap(Weak_Ptr& Ref)
		{
			auto ptr = _ptr;
			auto _size = size;
			_ptr = Ref._ptr;
			size = Ref.size;
			Ref._ptr = ptr;
			Ref.size = _size;
		}

		void Reset()
		{
			_ptr = nullptr;
			size = size_t();
		}

		T* Get() const
		{
			return _ptr;
		}

		template <size_t ArrSize>
		void Fill(T(&Array)[ArrSize])
		{
			for (size_t i = 0; i < ArrSize > size ? size : ArrSize; i++)
			{
				Array[i] = *(_ptr + i);
			}
		}

		T& operator [](size_t Index)
		{
			return *(_ptr + Index);
		}
	};
#pragma endregion Weak_Ptr
}