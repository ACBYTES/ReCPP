#ifndef SMART_POINTERS_H
#define SMART_POINTERS_H

#pragma once

#include <initializer_list>
#include <vector>
#include <mutex>
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
			Rvr.Release();
		}

		template <typename T1, enable_if_t<is_base_of_v<T, T1>, bool> = false>
		[[nodiscard]] Unique_Ptr(Unique_Ptr<T1>&& Rvr) noexcept
		{
			_ptr = (T*)Rvr.Get();
			Rvr.Release();
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
			Rvr.Release();
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
			size = 0;
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
	template <typename T, enable_if_t<is_array_v<T>, bool> = false>
	[[nodiscard]] auto Make_Unique(const size_t Size) -> Unique_Ptr<T>
	{
		using type = remove_array_t<T>;
		type* _init = new type[Size]{}; //Default constructed for possible const types.
		return Unique_Ptr<T>(_init, Size);
	}

	/*
	* Makes unique pointer pointing to an array initialized with the initializer list passed.
	*/
	template <typename T, enable_if_t<is_array_v<T> && !is_const_v<T>, bool> = false> //No const types allowed because the array is going to be filled with the initializer list.
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
	template <typename T, typename... ArgT, enable_if_t<!is_array_v<T>, bool> = false>
	[[nodiscard]] auto Make_Unique(ArgT... Arguments) -> Unique_Ptr<T>
	{
		auto _init = new T(Forward<ArgT>(Arguments)...);
		return Unique_Ptr<T>(_init);
	}
#pragma endregion Unique_Ptr

#pragma region Shared_Ptr
	struct IShared_Ref_Counter
	{
		virtual ~IShared_Ref_Counter()
		{
		}

		virtual bool Dead() = 0;
		virtual bool operator ==(void* Ptr) = 0;
		virtual IShared_Ref_Counter& operator ++() = 0;
		virtual IShared_Ref_Counter& operator --() = 0;
	};

	template <typename T>
	struct Shared_Ref_Counter final : public IShared_Ref_Counter
	{
	private:
		uint32_t _count = 1;
		T* _ptr = nullptr;

	public:
		Shared_Ref_Counter(T* Ptr) : _ptr(Ptr)
		{
		}

		~Shared_Ref_Counter()
		{
			delete _ptr;
		}

		bool Dead() override
		{
			return _count < 1;
		}

		bool operator ==(void* Ptr) override
		{
			return Ptr == _ptr;
		}

		IShared_Ref_Counter& operator ++() override
		{
			++_count;
			return *this;
		}

		IShared_Ref_Counter& operator --() override
		{
			--_count;
			return *this;
		}
	};

	template <typename T>
	struct Shared_Ref_Counter<T[]> final : public IShared_Ref_Counter
	{
	private:
		uint32_t _count = 1;
		T* _ptr = nullptr;

	public:
		Shared_Ref_Counter(T* Ptr) : _ptr(Ptr)
		{
		}

		~Shared_Ref_Counter()
		{
			delete[] _ptr;
		}

		bool Dead() override
		{
			return _count < 1;
		}

		bool operator ==(void* Ptr) override
		{
			return Ptr == _ptr;
		}

		IShared_Ref_Counter& operator ++() override
		{
			++_count;
			return *this;
		}

		IShared_Ref_Counter& operator --() override
		{
			--_count;
			return *this;
		}
	};

	struct Shared_Ptr_Container final
	{
	public:
		NO_DEFAULT_CONSTRUCTORS(Shared_Ptr_Container);
		template <typename> friend class Shared_Ptr;

	private:
		static std::vector<IShared_Ref_Counter*> references;
		static const size_t reserve_size = 4;

		static std::mutex referenceMutex;

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
				std::lock_guard<std::mutex> mLock(referenceMutex);
				for (auto i = references.begin(); i != references.end(); i++)
				{
					auto& ref = **i;
					if (ref == Ptr)
					{
						++ref;
						return;
					}
				}
				if (IsArray)
					references.push_back(new Shared_Ref_Counter<T[]>(Ptr));
				else
					references.push_back(new Shared_Ref_Counter<T>(Ptr));
			}
		}

		template <typename T>
		static void RemoveReference(T* Ptr)
		{
			if (Ptr)
			{
				std::lock_guard<std::mutex> mLock(referenceMutex);
				for (auto i = references.begin(); i != references.end(); i++)
				{
					auto& ref = **i;
					if (ref == Ptr)
					{
						if ((--(ref)).Dead())
						{
							delete (*i);
							references.erase(i);
						}
						return;
					}
				}
			}
		}
	};

	std::vector<IShared_Ref_Counter*> Shared_Ptr_Container::references = std::vector<IShared_Ref_Counter*>();
	std::mutex Shared_Ptr_Container::referenceMutex;

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

		template <typename T1, enable_if_t<is_base_of_v<T, T1>, bool> = false>
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
			size = 0;
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
	template <typename T, enable_if_t<is_array_v<T>, bool> = false>
	[[nodiscard]] auto Make_Shared(const size_t Size) -> Shared_Ptr<T>
	{
		using type = remove_array_t<T>;
		type* _init = new type[Size]{}; //Default constructed for possible const types.
		return Shared_Ptr<T>(_init, Size);
	}

	/*
	* Makes shared pointer pointing to an array initialized with the initializer list passed.
	*/
	template <typename T, enable_if_t<is_array_v<T> && !is_const_v<T>, bool> = false> //No const types allowed because the array is going to be filled with the initializer list.
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
	template <typename T, typename... ArgT, enable_if_t<!is_array_v<T>, bool> = false>
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
		[[nodiscard]] Weak_Ptr() //Empty pointer
		{
		}

		[[nodiscard]] Weak_Ptr(const Shared_Ptr<T>& Ref)
		{
			_ptr = Ref.Get();
		}

		template <typename T1, enable_if_t<is_base_of_v<T, T1>, bool> = false>
		[[nodiscard]] Weak_Ptr(const Shared_Ptr<T1>& Ref)
		{
			_ptr = (T*)Ref.Get();
		}

		template <typename T1, enable_if_t<is_base_of_v<T, T1>, bool> = false>
		[[nodiscard]] Weak_Ptr(const Weak_Ptr<T1>& Ref)
		{
			_ptr = (T*)Ref._ptr;
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
		[[nodiscard]] Weak_Ptr() //Empty pointer
		{
		}

		[[nodiscard]] Weak_Ptr(const Shared_Ptr<T[]>& Ref)
		{
			_ptr = Ref.Get();
			size = Ref.Size();
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

		size_t Size() const
		{
			return size;
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

#endif SMART_POINTERS_H