#pragma once
#include "main.hpp"

#include "util/debug/assert.hpp"
#include "util/debug/errors.hpp"

#include <atomic>
#include <thread>

namespace Shutdown
{	
	class Guard final
	{
	private:
		static inline std::atomic_uint activeGuards;

	public:
		static inline std::uint32_t GetActiveGuards() noexcept
		{
			return activeGuards;
		}

		inline Guard() noexcept
		{
			activeGuards++;
		}

		inline ~Guard()
		{
			activeGuards--;

			if (Main::IsInShutdown())
				std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	};
}

#define _SHUTDOWN_GUARD \
	auto _shutdown_guard = Shutdown::Guard();
