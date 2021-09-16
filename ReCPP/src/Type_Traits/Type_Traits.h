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
#pragma endregion is_array

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
#pragma endregion enable_if

	template <typename T>
	struct is_const
	{
		static constexpr bool value = is_same<const T, T>::value;
	};

	template <typename T>
	struct add_const
	{
		typedef const T type;
	};

	template <typename T>
	struct add_cv
	{
		typedef const volatile T type;
	};
}