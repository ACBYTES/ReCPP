#pragma once

namespace ACBYTES
{
#pragma region is_same
	template <typename T, typename T1>
	struct is_same
	{
		static constexpr bool value = false;
	};

	template <typename T>
	struct is_same<T, T>
	{
		static constexpr bool value = true;
	};

	template <typename T, typename T1>
	static constexpr bool is_same_v = is_same<T, T1>::value;
#pragma endregion is_same

#pragma region conditional
	template <typename T, typename T1, bool Value>
	struct conditional
	{
	};

	template <typename T, typename T1>
	struct conditional<T, T1, true>
	{
		typedef T result;
	};

	template <typename T, typename T1>
	struct conditional<T, T1, false>
	{
		typedef T1 result;
	};

	template <typename T, typename T1, bool Value>
	using conditional_t = typename conditional<T, T1, Value>::result;
#pragma endregion conditional

#pragma region is_pointer
	template <typename T>
	struct is_pointer
	{
		static constexpr bool value = false;
	};

	template <typename T>
	struct is_pointer<T*>
	{
		static constexpr bool value = true;
	};

	template <typename T>
	static constexpr bool is_pointer_v = is_pointer<T>::value;
#pragma endregion is_pointer

#pragma region is_array
	template <typename T>
	struct is_array
	{
		static constexpr bool value = false;
	};

	template <typename T>
	struct is_array<T[]>
	{
		static constexpr bool value = true;
	};

	template <typename T, size_t Size>
	struct is_array<T[Size]>
	{
		static constexpr bool value = true;
	};

	template <typename T>
	static constexpr bool is_array_v = is_array<T>::value;
#pragma endregion is_array

#pragma region remove_array
	template <typename T>
	struct remove_array
	{
		typedef T type;
	};

	template <typename T>
	struct remove_array<T[]>
	{
		typedef T type;
	};

	template <typename T, size_t Size>
	struct remove_array<T[Size]>
	{
		typedef T type;
	};

	template <typename T>
	using remove_array_t = typename remove_array<T>::type;
#pragma endregion remove_array

#pragma region enable_if
	template <bool V, typename T = void>
	struct enable_if
	{
	};

	template <typename T>
	struct enable_if<true, T>
	{
		typedef T type;
	};

	template <bool V, typename T = void>
	using enable_if_t = typename enable_if<V, T>::type;
#pragma endregion enable_if

#pragma region cv
	template <typename T>
	struct is_const
	{
		static constexpr bool value = is_same<const T, T>::value;
	};

	template <typename T>
	static constexpr bool is_const_v = is_const<T>::value;

	template <typename T>
	struct add_const
	{
		typedef const T type;
	};

	template <typename T>
	using add_const_t = typename add_const<T>::type;

	template <typename T>
	struct add_cv
	{
		typedef const volatile T type;
	};

	template <typename T>
	using add_cv_t = typename add_cv<T>::type;

	template <typename T>
	struct remove_const
	{
		typedef T type;
	};

	template <typename T>
	struct remove_const<const T>
	{
		typedef T type;
	};
	
	template <typename T>
	using remove_const_t = typename remove_const<T>::type;

	template <typename T>
	struct remove_volatile
	{
		typedef T type;
	};

	template <typename T>
	struct remove_volatile<volatile T>
	{
		typedef T type;
	};

	template <typename T>
	using remove_volatile_t = typename remove_volatile<T>::type;

	template <typename T>
	struct remove_cv
	{
		typedef typename remove_const<typename remove_volatile<T>::type>::type type;
	};

	template <typename T>
	using remove_cv_t = typename remove_cv<T>::type;
#pragma endregion cv

#pragma region reference_types
	template <typename T>
	struct is_lvalue_reference
	{
		static constexpr bool value = false;
	};

	template <typename T>
	struct is_lvalue_reference<T&>
	{
		static constexpr bool value = true;
	};

	template <typename T>
	static constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

	template <typename T>
	struct is_rvalue_reference
	{
		static constexpr bool value = false;
	};

	template <typename T>
	struct is_rvalue_reference<T&&>
	{
		static constexpr bool value = true;
	};

	template <typename T>
	static constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
#pragma endregion reference_types

#pragma region remove_reference
	template <typename T>
	struct remove_reference
	{
		typedef T type;
	};

	template <typename T>
	struct remove_reference<T&>
	{
		typedef T type;
	};

	template <typename T>
	struct remove_reference<T&&>
	{
		typedef T type;
	};

	template <typename T>
	using remove_reference_t = typename remove_reference<T>::type;
#pragma endregion remove_reference

#pragma region base_type
	template <typename T>
	struct base_type
	{
		typedef typename remove_array<typename remove_cv<typename remove_reference<T>::type>::type>::type type;
	};

	template <typename T>
	using base_type_t = typename base_type<T>::type;
#pragma endregion base_type

#pragma region Forward
	template <typename T>
	[[nodiscard]] constexpr T&& Forward(typename remove_reference<T>::type& R) noexcept
	{
		return (T&&)(R);
	}

	template <typename T, typename enable_if<!is_lvalue_reference<T>::value, bool>::type = false>
	[[nodiscard]] constexpr T&& Forward(typename remove_reference<T>::type&& RVR) noexcept
	{
		return (T&&)(RVR);
	}
#pragma endregion Forward

#pragma region Move
	template <typename T>
	[[nodiscard]] constexpr typename remove_reference<T>::type&& Move(T&& V)
	{
		return (typename remove_reference<T>::type&&)V;
	}
#pragma endregion Move

#pragma region is_base_of
	template <typename T>
	static constexpr bool TestBaseType(void*) noexcept
	{
		return false;
	}

	template <typename T>
	static constexpr bool TestBaseType(T*) noexcept
	{
		return true;
	}

	template <typename Base, typename Class>
	struct is_base_of
	{
		static constexpr bool value = TestBaseType<Base>(static_cast<Class*>(nullptr));
	};

	template <typename Base, typename Class>
	static constexpr bool is_base_of_v = is_base_of<Base, Class>::value;
#pragma endregion is_base_of

#pragma region is_convertible_to
	template <typename From, typename To>
	struct is_convertible_to
	{
		static constexpr bool value = is_same<decltype((To)(*(static_cast<From*>(nullptr)))), To>::value;
	};

	template <typename T>
	struct is_convertible_to<T, T>
	{
		static constexpr bool value = true;
	};

	template <typename From, typename To>
	static constexpr bool is_convertible_to_v = is_convertible_to<From, To>::value;
#pragma endregion is_convertible_to
}