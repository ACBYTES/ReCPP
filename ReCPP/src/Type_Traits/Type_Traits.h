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
}