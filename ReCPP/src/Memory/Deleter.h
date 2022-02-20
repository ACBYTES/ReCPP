//TODO

#ifndef DELETER_H
#define DELETER_H

#pragma once

namespace ACBYTES
{
	template <typename T>
	struct Default_Delete
	{
		constexpr Default_Delete() noexcept
		{
			static_assert(sizeof(T) > 0, "Unable to construct with incomplete type.");
		}

		template <typename T1, enable_if_t<is_convertible_v<T1, T> && (sizeof(T1) > 0), bool> = false>
		constexpr Default_Delete(const Default_Delete<T1>&) noexcept
		{
		}

		void operator()(T* Ptr) const
		{
			delete Ptr;
		}
	};

	template <typename T>
	struct Default_Delete<T[]>
	{
		constexpr Default_Delete() noexcept
		{
			static_assert(sizeof(T) > 0, "Unable to construct with incomplete type.");
		}

		template <typename T1, enable_if_t<is_convertible_v<T1, T> && (sizeof(T1) > 0), bool> = false>
		constexpr Default_Delete(const Default_Delete<T1>&) noexcept
		{
		}

		void operator()(T* Ptr) const
		{
			delete Ptr;
		}
	};
}

#endif DELETER_H