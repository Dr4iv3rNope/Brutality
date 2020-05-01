#include "memory.hpp"
#include "strings.hpp"

#include "debug/labels.hpp"

std::size_t Util::GetFuncSize(std::uintptr_t address, std::size_t cc_count)
{
	UTIL_DEBUG_ASSERT(address != 0);
	UTIL_DEBUG_ASSERT(cc_count != 0);

	std::size_t size{ 0 }, cc_detected{ 0 };

	while (cc_detected < cc_count)
	{
		size++;

		const auto& byte = *reinterpret_cast<const char*>(address + size);

		if (byte == '\xCC')
			cc_detected++;
		else
			cc_detected = 0;
	}

	return size - cc_detected;
}
