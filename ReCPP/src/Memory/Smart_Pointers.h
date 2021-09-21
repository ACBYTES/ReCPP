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
		bool _moved = false;

	public:

		[[nodiscard]] Unique_Ptr(std::nullptr_t = nullptr) //Empty pointer.
		{
		}

		[[nodiscard]] Unique_Ptr(T* Ptr) : _ptr(Ptr)
		{
		}

		~Unique_Ptr()
		{
			if (_ptr && !_moved)
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
		Unique_Ptr(Unique_Ptr&& R) noexcept
		{
			R._moved = true;
			_ptr = R._ptr;
		}
	};

	template <typename T>
	class Unique_Ptr<T[]>
	{
		T* _ptr = nullptr;
		size_t size;
		bool _moved = false;

	public:

		[[nodiscard]] Unique_Ptr(std::nullptr_t = nullptr) //Empty pointer.
		{
		}

		[[nodiscard]] Unique_Ptr(T* ArrPtr, size_t Size) : _ptr(ArrPtr), size(Size)
		{
		}

		~Unique_Ptr()
		{
			if (_ptr && !_moved)
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
		Unique_Ptr(Unique_Ptr&& R) noexcept
		{
			R._moved = true;
			_ptr = R._ptr;
			size = R.size;
		}
	};

	/*
	* Makes unique pointer pointing to an array with the size passed.
	*/
	template <typename T, typename ACBYTES::enable_if<ACBYTES::is_array<T>::value, bool>::type = false>
	[[nodiscard]] auto Make_Unique(const size_t Size)
	{
		using type = typename ACBYTES::remove_array<T>::type;
		type* _init = new type[Size]{}; //Initialization for possible const types.
		return Unique_Ptr<T>(_init, Size);
	}

	/*
	* Makes unique pointer pointing to an array initialized with the initializer list passed.
	*/
	template <typename T, typename ACBYTES::enable_if<ACBYTES::is_array<T>::value && !ACBYTES::is_const<T>::value, bool>::type = false> //No const types allowed because the array is going to be filled with the initializer list.
	[[nodiscard]] auto Make_Unique(std::initializer_list<typename ACBYTES::remove_array<T>::type> List) -> Unique_Ptr<T>
	{
		using type = typename ACBYTES::remove_array<T>::type;
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
	template <typename T, typename... ArgT, typename ACBYTES::enable_if<!ACBYTES::is_array<T>::value, bool>::type = false>
	[[nodiscard]] auto Make_Unique(ArgT... Arguments)
	{
		auto _init = new T(Forward<ArgT>(Arguments)...);
		return Unique_Ptr<T>(_init);
	}
#pragma endregion Unique_Ptr

#pragma region Shared_Ptr

	struct Shared_Reference_Data final
	{
	private:
		uint32_t _count = 1;
		void* _ptr = nullptr;
		bool _array;

	public:

		Shared_Reference_Data(void* Ptr, bool IsArray) : _ptr(Ptr), _array(IsArray)
		{
		}

		~Shared_Reference_Data()
		{
			if (_ptr && _count < 1)
			{
				if (_array)
					delete[] _ptr;
				else
					delete _ptr;
			}
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

		static void AddNewReference(void* Ptr, bool IsArray = false)
		{
			for (auto i = references.begin(); i != references.end(); i++)
			{
				if (*i == Ptr)
				{
					++(*i);
					return;
				}
			}
			references.push_back(Shared_Reference_Data(Ptr, IsArray));
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
		T* _ptr = nullptr;

	public:

		[[nodiscard]] Shared_Ptr(std::nullptr_t = nullptr) //Empty pointer.
		{
		}

		[[nodiscard]] Shared_Ptr(T* Ptr) : _ptr(Ptr)
		{
			Shared_Ptr_Container::AddNewReference((void*)Ptr);
		}

		Shared_Ptr(const Shared_Ptr& Ref)
		{
			Shared_Ptr_Container::AddNewReference((void*)Ref._ptr);
			_ptr = Ref._ptr;
		}

		Shared_Ptr(Shared_Ptr&& Rvf) noexcept
		{
			Shared_Ptr_Container::AddNewReference((void*)Rvf._ptr);
			_ptr = Rvf._ptr;
		}

		~Shared_Ptr()
		{
			if (_ptr)
				Shared_Ptr_Container::RemoveReference((void*)_ptr);
		}

		void Swap(Shared_Ptr& Ref)
		{
			auto ptr = _ptr;
			_ptr = Ref._ptr;
			Ref._ptr = ptr;
		}

		void Reset(T* Ptr = nullptr)
		{
			if (_ptr)
				Shared_Ptr_Container::RemoveReference((void*)_ptr);

			if (Ptr)
				Shared_Ptr_Container::AddNewReference((void*)Ptr);

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
			Shared_Ptr_Container::AddNewReference((void*)Ptr, true);
		}

		Shared_Ptr(const Shared_Ptr& Ref)
		{
			Shared_Ptr_Container::AddNewReference((void*)Ref._ptr, true);
			_ptr = Ref._ptr;
			size = Ref.Size();
		}

		Shared_Ptr(Shared_Ptr&& Rvf) noexcept
		{
			Shared_Ptr_Container::AddNewReference((void*)Rvf._ptr, true);
			_ptr = Rvf._ptr;
			size = Rvf.Size();
		}

		~Shared_Ptr()
		{
			if (_ptr)
				Shared_Ptr_Container::RemoveReference((void*)_ptr);
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
	template <typename T, typename ACBYTES::enable_if<ACBYTES::is_array<T>::value, bool>::type = false>
	[[nodiscard]] auto Make_Shared(const size_t Size) -> Shared_Ptr<T>
	{
		using type = typename ACBYTES::remove_array<T>::type;
		type* _init = new type[Size]{}; //Initialization for possible const types.
		return Shared_Ptr<T>(_init, Size);
	}

	/*
	* Makes shared pointer pointing to an array initialized with the initializer list passed.
	*/
	template <typename T, typename ACBYTES::enable_if<ACBYTES::is_array<T>::value && !ACBYTES::is_const<T>::value, bool>::type = false> //No const types allowed because the array is going to be filled with the initializer list.
	[[nodiscard]] auto Make_Shared(std::initializer_list<typename ACBYTES::remove_array<T>::type> List) -> Shared_Ptr<T>
	{
		using type = typename ACBYTES::remove_array<T>::type;
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
	template <typename T, typename... ArgT, typename ACBYTES::enable_if<!ACBYTES::is_array<T>::value, bool>::type = false>
	[[nodiscard]] auto Make_Shared(ArgT... Arguments)
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

		Weak_Ptr()
		{
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