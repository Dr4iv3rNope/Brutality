#pragma once
#include <type_traits>

namespace Util
{
	// ^V - value that used to check flags (can be enum or number)
	// ^F - flags value (always must be number)
	template <typename V = int, typename F = int>
	struct Flags
	{
		static_assert(std::is_arithmetic<F>(), "F must be a number!");
		static_assert(!std::is_floating_point<F>(), "F cant be a float!");
		static_assert(!std::is_same<bool, F>(), "F cant be a bool!");
		static_assert(!std::is_enum<F>(), "F cant be a enum!");

		static_assert(std::is_arithmetic<V>() || std::is_enum<V>(), "V must be a number or enum!");
		static_assert(!std::is_floating_point<V>(), "V cant be a float!");
		static_assert(!std::is_same<bool, V>(), "V cant be a bool!");

		F flags;

		constexpr inline Flags(F flags) noexcept
			: flags { flags } {}

		constexpr explicit operator F() const noexcept { return flags; }

		constexpr inline Flags& SetFlag(V flag) noexcept { flags |= F(flag); return *this; }
		constexpr inline Flags& RemoveFlag(V flag) noexcept { flags &= ~F(flag); return *this; }
		constexpr inline bool HasFlag(V flag) const noexcept { return flags & F(flag); }

		// checks if flag is set
		constexpr inline bool operator&(V flag) const noexcept { return HasFlag(flag); }

		// adds new flag to new Flags structure instance
		constexpr inline Flags operator|(V flag) const noexcept { return Flags(flags).SetFlag(flag); }

		// set flags to current structure
		constexpr inline Flags& operator|=(V flag) noexcept { return SetFlag(flag); }
		
		constexpr inline Flags& operator&=(V flag) noexcept { return flags &= flag; }
	};
}

/*
#define UTIL_FLAGS(flagName) \
	using flagName = int; \
	enum flagName##_
*/

#define UTIL_FLAGS(flagName) \
	enum flagName##_; \
	using flagName = Util::Flags<flagName##_>; \
	enum flagName##_

#define UTIL_MAKE_FLAG(flagName, name, value) \
	flagName##_##name = value
