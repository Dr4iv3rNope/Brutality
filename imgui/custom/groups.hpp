#pragma once
#include <string>

#include "../../util/flags.hpp"

namespace ImGui
{
	namespace Custom
	{
		UTIL_FLAGS(GroupFlags)
		{
			UTIL_MAKE_BIT_FLAG(GroupFlags, ReturnIsOpened, 0)
		};

		// return if selectable has been clicked
		//
		// example of usage:
		// ```
		// static bool options_opened = false;
		// BeginGroup("Options", options_opened);
		// 
		// if (options_opened) { ... EndGroup(); }
		// ```
		extern bool BeginGroup(
			const std::string& label,
			bool& is_open,
			GroupFlags flags = 0
		) noexcept;

		extern void EndGroup() noexcept;
	}
}
