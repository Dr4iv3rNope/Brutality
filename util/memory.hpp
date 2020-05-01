#pragma once
#include <cstdint>
#include <cstddef>

namespace Util
{
	extern std::size_t GetFuncSize(std::uintptr_t address, std::size_t cc_count = 1);

	inline std::uintptr_t GetRelAddress(std::uintptr_t from, std::uintptr_t to) noexcept
	{
		return to - from - 5;
	}

	template <typename T>
	inline T GetAbsAddress(std::uintptr_t op) noexcept
	{
		return T(op + 5 + *(int*)(op + 1));
	}

	template <typename T, int C = 1>
	inline T FollowPointer(T ptr) noexcept
	{
		static_assert(C > 0);

		for (int count = 0; count < C; count++)
			ptr = *(T*)ptr;

		return ptr;
	}
}
